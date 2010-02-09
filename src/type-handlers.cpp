/* Largely adapted from QtRuby */

/* Type conversion thoughts:

   Need to cleanup the conversion process. Can probably define
   everything in the TypeHandler array with better use of
   templates. Base this on the 'as' function in Rcpp. For C++ -> SEXP,
   just overload SEXP as(C++). For SEXP -> C++, use their templated
   C++ as<C++>(SEXP).
   
   See types.txt for a list of types we need to handle.

   Should probably factor out the actual conversions implemented in
   this file, so that we can use them from the QVariant converter.
   
   Enums: It does not seem like Smoke provides the string name of enum
   values. Qt does not help much either, because QMetaEnum only works
   for enums defined in a class. Many are not, and how would we get
   the class anyway?

   Values: Many value types (e.g. QRectF) can be handled through
   high-level type conversion. The question is, which ones? Some
   types, e.g. QString, have obvious analogs in R. However, if we have
   to represent an object as a list (or probably better) S4 class,
   what do we gain? We can break this down by usability,
   maintainability and performance.
   
     Usability: The interface is more complex if we mix the dynamic Qt
     API and S4. Thus, usability favors sticking with Qt references,
     but only slightly, because the user never really needs to know
     that they are S4 or pointers.

     Maintainability: Clearly, it's easier not to convert to S4.
     
     Performance: There is overhead to the conversions, which would
     slow the S4 approach. However, calling Qt methods to access
     fields is slower than accessing the slots of an S4 object. It
     comes down to whether one is passing the object to another Qt
     function or merely accessing the fields of the object.
     
   If the question is performance, we can optimize for specific
   use-cases (but beware of premature optimization). For now, we will
   avoid any complex conversions and stick to what is natural/obvious.

   In the below, the '*' indicates an obvious/natural mapping.
     
   Converters that we have:
   
     *QRectF -> 2x2 matrix (par() uses a flat vector in same order),
     *QPointF -> 2-vector,
     *QSizeF -> 2-vector,
     *QMatrix (replace with QTransform),
     QFont -> list,
     *QColor -> 4x1 matrix,
     *QVariant -> R value,
     *QString -> character, *QByteArray -> character
     
   Converters that we currently lack:
     QBrush/QPen: something in qtgui, needs to be expanded
     QKeySequence: from string or standard key (integer)
     *QUrl/QHostAddress: from strings
     QPolygon(F)/QLine(F): from 2 column matrix
     *QTransform: 3x3 matrix
     
     On an as-needed basis:
     *QDBusSignature/QLatin1String: from string
     *QDate/QTime: from R date and time objects
     QStyleOption (and subclasses): from an R list
     QTextFormat/QPalette/QTextOption: similar to QBrush/QPen
     *QBitArray: from an R raw vector
     QNetworkCacheMetaData/QUrlInfo: R list?
     QSslKey/QSslCipher: as character + attributes for other fields?
     QXmlStreamAttribute: as character + attributes
     QTextLength: as integer + plus type attribute
     *QUuid, QXmlStream*Declaration: as string
     QHttpHeader: character vector + attributes
     QNetworkAddressEntry: IP as character + attributes
     QNetworkCookie: named character + attributes
     QSizePolicy: two integers
     QSourceLocation: three elements of a list
     QSslError: two integers
     QDBusError/QDBusReply/QSslConfiguration/QXmlParseException: list
     QFontInfo/QPrinterInfo/QHostInfo: list
     (webkit) QWebHistoryItem/QWebSecurityOrigin/QWebDatabase: list 
     QXmlAttributes: character vector + attributes?
     *QTableWidgetSelectionRange: like QRect
     *QMargins: like QRect
     *QMatrix4x4: as R matrix
     *QVector(2D,3D,4D): as R vectors
     *QGenericMatrix: as R matrix

   Many of the types marked 'list', and others, should probably become
   S4 objects. Makes validation, documentation easier.
   
   Many of the const-ref types will just be copied, and then marked
   for destruction when they drop out of R scope.
   
   References to primitives: out parameters or arrays? QtRuby treats
   them as out parameters, but QFontMetrics::size() is one example
   where int* is an array.

   Pointers: the issue is memory management. If we receive a const
   pointer, it is clear that we should not free it later. However, many
   pointers are passed non-const, even when Qt owns them. In the case
   of some types, like QObjects and QGraphicsItems, we can detect
   ownership by the presence of a parent. But even QIODevice*, a
   QObject *, is often a client of objects that are not its
   parent. The choices, basically, are:
     1) Do not free any memory, unless we are sure we own it. Require
        the R user to clean up.
     2) Free memory aggressively. Require the R user to understand the
        Qt memory management policy to keep objects around.
     3) Allow the user to select one of the above. Makes things more
        complicated, though.
   Faced with the options, #2 seems easiest, for the same reason Qt
   supports parent ownership -- it's convenient and eliminates
   book-keeping. Not every object will support parent ownership, so a
   reference will need to be stored in an environment somewhere and
   rm()'d when no longer needed. Usually, it is not a big deal. For
   example, objects like QPainter and QEvent are not often referred to
   by other objects. An exception would be QTextCodec.
 */

/* TODO:
   - Conditional compilation of OpenGL, QtXml, QtNetwork, etc, types
   - More scoring functions, as needed
 */

#include <QtCore/qdir.h>
#include <QtCore/qhash.h>
#include <QtCore/qlinkedlist.h>
#include <QtCore/qmetaobject.h>
#include <QtCore/qobject.h>
#include <QtCore/qpair.h>
#include <QtCore/qprocess.h>
#include <QtCore/qregexp.h>
#include <QtCore/qstring.h>
#include <QtCore/qtextcodec.h>
#include <QtCore/qurl.h>
#include <QtGui/qabstractbutton.h>
#include <QtGui/qaction.h>
#include <QtGui/qapplication.h>
#include <QtGui/qdockwidget.h>
#include <QtGui/qevent.h>
#include <QtGui/qlayout.h>
#include <QtGui/qlistwidget.h>
#include <QtGui/qpainter.h>
#include <QtGui/qpalette.h>
#include <QtGui/qpixmap.h>
#include <QtGui/qpolygon.h>
#include <QtGui/qtabbar.h>
#include <QtGui/qtablewidget.h>
#include <QtGui/qtextedit.h>
#include <QtGui/qtextlayout.h>
#include <QtGui/qtextobject.h>
#include <QtGui/qtoolbar.h>
#include <QtGui/qtreewidget.h>
#include <QtGui/qwidget.h>

#ifdef QT_QTNETWORK
#include <QtNetwork/qhostaddress.h>
#include <QtNetwork/qnetworkinterface.h>
#include <QtNetwork/qurlinfo.h>
#endif

#if QT_VERSION >= 0x40200
#include <QtGui/qgraphicsitem.h>
#include <QtGui/qgraphicslayout.h>
#include <QtGui/qgraphicsscene.h>
#include <QtGui/qgraphicswidget.h>
#include <QtGui/qstandarditemmodel.h>
#include <QtGui/qundostack.h>
#endif

#if QT_VERSION >= 0x40300
#include <QtGui/qmdisubwindow.h>
#ifdef QT_QTNETWORK
#include <QtNetwork/qsslcertificate.h>
#include <QtNetwork/qsslcipher.h>
#include <QtNetwork/qsslerror.h>
#endif
#ifdef QT_QTXML
#include <QtXml/qxmlstream.h>
#endif
#endif

#if QT_VERSION >= 0x040400
#include <QtGui/qprinterinfo.h>
#ifdef QT_QTNETWORK
#include <QtNetwork/qnetworkcookie.h>
#endif
#endif

#if QT_VERSION >= 0x040600
#include <QAbstractAnimation>
#include <QAbstractState>
#ifdef QT_QTOPENGL
#include <QGLShader>
#endif
#endif

#include <smoke.h>
#include "type-handlers.hpp"
#undef isNull // R causing trouble again

template <class T>
static void marshal_it(MethodCall *m)
{
  switch(m->mode()) {
  case MethodCall::RToSmoke:
    marshal_from_sexp<T>(m);
    break;
 
  case MethodCall::SmokeToR:
    marshal_to_sexp<T>( m );
    break;
				
  default:
    m->unsupported();
    break;
  }
}

void marshal_basetype(MethodCall *m)
{
  switch(m->type().elem()) {

  case Smoke::t_bool:
    marshal_it<bool>(m);
    break;

  case Smoke::t_char:
    marshal_it<signed char>(m);
    break;
		
  case Smoke::t_uchar:
    marshal_it<unsigned char>(m);
    break;
 
  case Smoke::t_short:
    marshal_it<short>(m);
    break;
      
  case Smoke::t_ushort:
    marshal_it<unsigned short>(m);
    break;

  case Smoke::t_int:
    marshal_it<int>(m);
    break;
		
  case Smoke::t_uint:
    marshal_it<unsigned int>(m);
    break;
 
  case Smoke::t_long:
    marshal_it<long>(m);
    break;

  case Smoke::t_ulong:
    marshal_it<unsigned long>(m);
    break;
 
  case Smoke::t_float:
    marshal_it<float>(m);
    break;

  case Smoke::t_double:
    marshal_it<double>(m);
    break;

  case Smoke::t_enum:
    marshal_it<SmokeEnumWrapper>(m);
    break;
     
  case Smoke::t_class:
    marshal_it<SmokeClassWrapper>(m);
    break;

  default:
    m->unsupported();
    break;	
  }

}

/* C++ compilers select overloaded methods by ranking each argument in
   terms of its implicit conversions. There are three different types
   of implicit conversions: standard, user and ellipsis. We only
   consider the former, as the others probably are not relevant for
   Qt. Within standard conversions, there are three ranks: exact,
   promotion and conversion. We assign these scores 3, 2 and 1,
   respectively, while 0 indicates no conversion and -1 indicates an
   error (e.g. unsupported type). The method with better or same
   ranks for all of its parameters is selected. If there is a tie for
   the best method, there is an error (in our code).
*/
int scoreArg_basetype(SEXP arg, const SmokeType &type) {
  int score = 0;
  SEXP value = arg;
  int rtype = TYPEOF(value);
  unsigned short elem = type.elem();
  // FIXME: we are not considering pointers to primitives;
  // We do not know whether they are "out" params or arrays
  switch(rtype) { // try the simple cases first
  case RAWSXP:
    switch(elem) {
    case Smoke::t_uchar:
      score = 3;
      break;
    case Smoke::t_short:
    case Smoke::t_ushort:
      score = 2;
      break;
    case Smoke::t_int:
    case Smoke::t_uint:
    case Smoke::t_long:
    case Smoke::t_ulong:
    case Smoke::t_float:
    case Smoke::t_double:
      score = 1;
      break;
    default:
      break;
    }
    break;
  case INTSXP:
    switch(elem) {
    case Smoke::t_enum:
      if (inherits(value, "QtEnum"))
        score = 2;
      else score = 1;
      break;
    case Smoke::t_int:
      score = 3;
      break;
    case Smoke::t_uint:
    case Smoke::t_long:
    case Smoke::t_ulong:
    case Smoke::t_float:
    case Smoke::t_double:
      score = 2;
      break;
    case Smoke::t_short:
    case Smoke::t_ushort:
    case Smoke::t_uchar:
      score = 1;
      break;
    default:
      break;
    }
    break;
  case REALSXP:
    switch(elem) {
    case Smoke::t_double:
      score = 3;
      break;
    case Smoke::t_float:
    case Smoke::t_uint: // to distinguish int/uint
      score = 2;
      break;
    case Smoke::t_int:
    case Smoke::t_long:
    case Smoke::t_ulong:
    case Smoke::t_short:
    case Smoke::t_ushort:
    case Smoke::t_uchar:
    case Smoke::t_enum:
      score = 1;
      break;
    default:
      break;
    }
    break;
  case STRSXP:
    if (elem == Smoke::t_char && strlen(CHAR(asChar(value))) == 1)
      score = 2;
    break;
  case ENVSXP:
    if (elem == Smoke::t_class) {
      SmokeObject *o = SmokeObject::fromSexp(value);
      if (o) {
        const char *smokeClass = type.name();
        if (o->className() == smokeClass)
          score = 3;
        else if (o->instanceOf(smokeClass))
          score = 2;
      }
    }
    break;
  case NILSXP:
    if (elem == Smoke::t_class)
      score = 1;
    break;
  default:
    break;
  }
  return score;
}

int scoreArg_byClass(SEXP arg, const SmokeType &type) {
  const char *className = CHAR(asChar(getAttrib(arg, R_ClassSymbol)));
  if (!qstrcmp(type.name(), className))
    return 3;
  if (inherits(arg, type.name()))
    return 2;
  return 0;
}

int scoreArg_unknown(SEXP /*arg*/, const SmokeType &type) {
  error("unable to score argument of type '%s'", type.name());
  return 0;
}

void marshal_void(MethodCall * /*m*/) {}
void marshal_unknown(MethodCall *m) {
  m->unsupported();
}

static void marshal_doubleR(MethodCall *m) {
  switch(m->mode()) {
  case MethodCall::RToSmoke:
    {
      SEXP rv = m->sexp();
      double * d = new double;
      *d = asReal(rv);
      m->item().s_voidp = d;
      m->marshal();
      if (m->cleanup()) {
        delete d;
      } else {
        m->item().s_voidp = new double((double)asReal(rv));
      }
    }
    break;
  case MethodCall::SmokeToR:
    {
      double *dp = (double*)m->item().s_voidp;
      SEXP rv = m->sexp();
      if (dp == 0) {
        rv = R_NilValue;
        break;
      }
      m->setSexp(ScalarReal(*dp));
      m->marshal();
      if (!m->type().isConst()) {
        *dp = asReal(m->sexp());
      }
    }
    break;
  default:
    m->unsupported();
    break;
  }
}

static void marshal_QString(MethodCall *m) {
  switch(m->mode()) {
  case MethodCall::RToSmoke:
    { 
      QString *s = NULL;
      if(!m->type().isPtr() || m->sexp() != R_NilValue) {
        s = new QString(sexp2qstring(m->sexp()));
      }
      
      m->item().s_voidp = s;
      m->marshal();

      if (m->cleanup())
        delete s;
      
      if (!m->type().isStack() && !m->type().isConst() &&
          m->sexp() != R_NilValue && !s->isNull())
      { 
        m->setSexp(qstring2sexp(*s));
      }
    }
    break;

  case MethodCall::SmokeToR:
    {
      QString *s = (QString*)m->item().s_voidp;
      if(s) {
        if (s->isNull()) {
          m->setSexp(R_NilValue);
        } else {
          m->setSexp(qstring2sexp(*s));
        }
        if(m->cleanup()) {
          delete s;
        }
      } else {
        m->setSexp(R_NilValue);
      }
    }
    break;
 
  default:
    m->unsupported();
    break;
  }
}

static int scoreArg_QString(SEXP arg, const SmokeType &/*type*/) {
  if (TYPEOF(arg) == STRSXP)
    return 3;
  else return 0;
}

static int scoreArg_QStringList(SEXP arg, const SmokeType &/*type*/) {
  if (TYPEOF(arg) == STRSXP) {
    if (length(arg) > 1)
      return 3;
    return 2;
  }
  else return 0;
}

void marshal_QDBusVariant(MethodCall *m) {
  switch(m->mode()) {
  case MethodCall::RToSmoke: 
    {
      SEXP v = m->sexp();
      if (v == R_NilValue) {
        m->item().s_voidp = 0;
        break;
      }

      SmokeObject *o = SmokeObject::fromSexp(v);
      if (!o || !o->ptr()) {
        if (m->type().isRef()) {
          m->unsupported();
        }
        m->item().s_class = 0;
        break;
      }
      m->item().s_class = o->ptr();
      break;
    }

  case MethodCall::SmokeToR: 
    {
      if (m->item().s_voidp == 0) {
        m->setSexp(R_NilValue);
        break;
      }

      void *p = m->item().s_voidp;
      SEXP obj =
        SmokeObject::fromPtr(p, m->smoke(), "QDBusVariant",
                             m->type().isStack())->sexp();

#ifdef DEBUG
      Rprintf("allocating %s %p -> %p\n", "Qt::DBusVariant", p, (void*)obj);
#endif

      m->setSexp(obj);
      break;
    }
	
  default:
    m->unsupported();
    break;
  }
}

void marshal_QVariant(MethodCall *m) {
  switch(m->mode()) {
  case MethodCall::RToSmoke: 
    {
      SEXP v = m->sexp();
      QVariant variant = asQVariant(v);
      m->item().s_class = &variant;
      break;
    }

  case MethodCall::SmokeToR: 
    {
      QVariant variant = *(const QVariant *)m->item().s_voidp;
      SEXP obj = asRVariant(variant);
      m->setSexp(obj);
      break;
    }
	
  default:
    m->unsupported();
    break;
  }
}

static void marshal_charP_array(MethodCall *m) {
  switch(m->mode()) {
  case MethodCall::RToSmoke:
    {
      SEXP arglist = m->sexp();
      if (arglist == R_NilValue
          || TYPEOF(arglist) != STRSXP
          || length(arglist) == 0 )
        {
          m->item().s_voidp = 0;
          break;
        }

      char **argv = new char *[length(arglist) + 1];
      long i;
      for(i = 0; i < length(arglist); i++) {
        SEXP item = STRING_ELT(arglist, i);
        const char *s = CHAR(asChar(item));
        argv[i] = new char[strlen(s) + 1];
        strcpy(argv[i], s);
      }
      argv[i] = 0;
      m->item().s_voidp = argv;
      m->marshal();

      PROTECT(arglist = allocVector(STRSXP, length(arglist)));
      for(i = 0; argv[i]; i++) {
        SET_STRING_ELT(arglist, i, mkChar(argv[i]));
      }
      UNPROTECT(1);
      m->setSexp(arglist);
    }
    break;
  default:
    m->unsupported();
    break;
  }
}

void marshal_QStringList(MethodCall *m) {
  switch(m->mode()) {
  case MethodCall::RToSmoke: 
    {
      SEXP list = m->sexp();
      if (TYPEOF(list) != STRSXP) {
        m->item().s_voidp = 0;
        break;
      }

      int count = length(list);
      QStringList *stringlist = new QStringList;

      for(long i = 0; i < count; i++) {
        SEXP item = STRING_ELT(list, i);
        stringlist->append(sexp2qstring(item));
      }

      m->item().s_voidp = stringlist;
      m->marshal();
      
      if (stringlist != 0 && !m->type().isConst()) {
        PROTECT(list = allocVector(STRSXP, length(list)));
        for (int i = 0; i < stringlist->size(); i++)
          SET_STRING_ELT(list, i, qstring2sexp(stringlist->at(i)));
        UNPROTECT(1);
        m->setSexp(list);
      }
			
      if (m->cleanup()) {
        delete stringlist;
      }
   
      break;
    }

  case MethodCall::SmokeToR: 
    {
      QStringList *stringlist = static_cast<QStringList *>(m->item().s_voidp);
      if (!stringlist) {
        m->setSexp(R_NilValue);
        break;
      }

      SEXP av;
      PROTECT(av = allocVector(STRSXP, stringlist->size()));
      for (int i = 0; i < length(av); i++)
        SET_STRING_ELT(av, i, qstring2sexp(stringlist->at(i)));
      UNPROTECT(1);
      
      m->setSexp(av);

      if (m->cleanup()) {
        delete stringlist;
      }

    }
    break;
  default:
    m->unsupported();
    break;
  }
}


void marshal_QByteArrayList(MethodCall *m) {
  switch(m->mode()) {
  case MethodCall::RToSmoke: 
    {
      SEXP list = m->sexp();
      if (TYPEOF(list) != STRSXP) {
        m->item().s_voidp = 0;
        break;
      }

      int count = length(list);
      QList<QByteArray> *stringlist = new QList<QByteArray>;

      for(long i = 0; i < count; i++) {
        SEXP item = VECTOR_ELT(list, i);
        stringlist->append(QByteArray(CHAR(asChar(item)), length(item)));
      }

      m->item().s_voidp = stringlist;
      m->marshal();

      if (!m->type().isConst()) {
        PROTECT(list = allocVector(STRSXP, length(list)));
        for (int i = 0; i < length(list); i++)
          SET_STRING_ELT(list, i, qstring2sexp(stringlist->at(i)));
        UNPROTECT(1);
        m->setSexp(list);
      }

      if(m->cleanup()) {
        delete stringlist;
      }
      break;
    }
  case MethodCall::SmokeToR: 
    {
      QList<QByteArray> *stringlist =
        static_cast<QList<QByteArray>*>(m->item().s_voidp);
      if(!stringlist) {
        m->setSexp(R_NilValue);
        break;
      }

      SEXP av;
      PROTECT(av = allocVector(STRSXP, stringlist->size()));
      for (int i = 0; i < length(av); i++)
        SET_STRING_ELT(av, i, qstring2sexp(stringlist->at(i)));
      UNPROTECT(1);

      m->setSexp(av);

      if (m->cleanup()) {
        delete stringlist;
      }
    }
    break;
  default:
    m->unsupported();
    break;
  }
}

void marshal_CharStarList(MethodCall *m) {
  switch(m->mode()) {
  case MethodCall::RToSmoke:
    {
      SEXP av = m->sexp();
      if (TYPEOF(av) != STRSXP) {
        m->item().s_voidp = 0;
        break;
      }
      int count = length(av);
      QList<const char*> *list = new QList<const char*>;
      long i;
      for(i = 0; i < count; i++) {
        SEXP item = STRING_ELT(av, i);
        list->append(CHAR(asChar(item)));
      }

      m->item().s_voidp = list;
    }
    break;
  case MethodCall::SmokeToR:
    {
      QList<const char*> *list = (QList<const char*>*)m->item().s_voidp;
      if (list == 0) {
        m->setSexp(R_NilValue);
        break;
      }

      SEXP av;
      PROTECT(av = allocVector(STRSXP, list->size()));
      for (int i = 0; i < length(av); i++)
        SET_STRING_ELT(av, i, qstring2sexp(list->at(i)));
      UNPROTECT(1);
		
      m->setSexp(av);
      m->marshal();
    }
    break;
  default:
    m->unsupported();
    break;
  }
}

void marshal_intList(MethodCall *m) {
  switch(m->mode()) {
  case MethodCall::RToSmoke:
    {
      SEXP list = m->sexp();
      if (TYPEOF(list) != INTSXP) {
        m->item().s_voidp = 0;
        break;
      }
      int count = length(list);
      QList<int> *valuelist = new QList<int>;
      long i;
      for(i = 0; i < count; i++) {
        valuelist->append(INTEGER(list)[i]);
      }

      m->item().s_voidp = valuelist;
      m->marshal();
      
      if (!m->type().isConst()) {
        list = allocVector(INTSXP, count);
        for (int i = 0; i < count; i++)
          INTEGER(list)[i] = valuelist->at(i);
        m->setSexp(list);
      }

      if (m->cleanup()) {
        delete valuelist;
      }
    }
    break;
  case MethodCall::SmokeToR:
    {
      QList<int> *valuelist = (QList<int>*)m->item().s_voidp;
      if(!valuelist) {
        m->setSexp(R_NilValue);
        break;
      }

      SEXP av = allocVector(INTSXP, valuelist->size());
      for (int i = 0; i < length(av); i++)
        INTEGER(av)[i] = valuelist->at(i);
      m->setSexp(av);
      
      m->marshal();

      if (m->cleanup()) {
        delete valuelist;
      }
    }
    break;
  default:
    m->unsupported();
    break;
  }
}


void marshal_uintList(MethodCall *m) {
  switch(m->mode()) {
  case MethodCall::RToSmoke:
    {
      SEXP list = m->sexp();
      if (TYPEOF(list) != REALSXP) {
        m->item().s_voidp = 0;
        break;
      }
      int count = length(list);
      QList<uint> *valuelist = new QList<uint>;
      long i;
      for(i = 0; i < count; i++) {
        valuelist->append(REAL(list)[i]);
      }

      m->item().s_voidp = valuelist;
      m->marshal();
      
      if (!m->type().isConst()) {
        list = allocVector(REALSXP, count);
        for (int i = 0; i < count; i++)
          REAL(list)[i] = valuelist->at(i);
        m->setSexp(list);
      }

      if (m->cleanup()) {
        delete valuelist;
      }
    }
    break;
  case MethodCall::SmokeToR:
    {
      QList<uint> *valuelist = (QList<uint>*)m->item().s_voidp;
      if(!valuelist) {
        m->setSexp(R_NilValue);
        break;
      }

      SEXP av = allocVector(REALSXP, valuelist->size());
      for (int i = 0; i < length(av); i++)
        REAL(av)[i] = valuelist->at(i);
      m->setSexp(av);
      
      m->marshal();

      if (m->cleanup()) {
        delete valuelist;
      }
    }
    break;
  default:
    m->unsupported();
    break;
  }
}

void marshal_doubleList(MethodCall *m) {
  switch(m->mode()) {
  case MethodCall::RToSmoke:
    {
      SEXP list = m->sexp();
      if (TYPEOF(list) != REALSXP) {
        m->item().s_voidp = 0;
        break;
      }
      int count = length(list);
      QList<qreal> *valuelist = new QList<qreal>;
      long i;
      for(i = 0; i < count; i++) {
        valuelist->append(REAL(list)[i]);
      }

      m->item().s_voidp = valuelist;
      m->marshal();
      
      if (!m->type().isConst()) {
        list = allocVector(REALSXP, count);
        for (int i = 0; i < count; i++)
          REAL(list)[i] = valuelist->at(i);
        m->setSexp(list);
      }

      if (m->cleanup()) {
        delete valuelist;
      }
    }
    break;
  case MethodCall::SmokeToR:
    {
      QList<qreal> *valuelist = (QList<qreal>*)m->item().s_voidp;
      if(!valuelist) {
        m->setSexp(R_NilValue);
        break;
      }

      SEXP av = allocVector(REALSXP, valuelist->size());
      for (int i = 0; i < length(av); i++)
        REAL(av)[i] = valuelist->at(i);
      m->setSexp(av);
      
      m->marshal();

      if (m->cleanup()) {
        delete valuelist;
      }
    }
    break;
  default:
    m->unsupported();
    break;
  }
}

void marshal_doubleVector(MethodCall *m) {
  switch(m->mode()) {
  case MethodCall::RToSmoke:
    {
      SEXP list = m->sexp();
      if (TYPEOF(list) != REALSXP) {
        m->item().s_voidp = 0;
        break;
      }
      int count = length(list);
      QVector<qreal> *valuelist = new QVector<qreal>;
      long i;
      for(i = 0; i < count; i++) {
        valuelist->append(REAL(list)[i]);
      }

      m->item().s_voidp = valuelist;
      m->marshal();
      
      if (!m->type().isConst()) {
        list = allocVector(REALSXP, count);
        for (int i = 0; i < count; i++)
          REAL(list)[i] = valuelist->at(i);
        m->setSexp(list);
      }

      if (m->cleanup()) {
        delete valuelist;
      }
    }
    break;
  case MethodCall::SmokeToR:
    {
      QVector<qreal> *valuelist = (QVector<qreal>*)m->item().s_voidp;
      if(!valuelist) {
        m->setSexp(R_NilValue);
        break;
      }

      SEXP av = allocVector(REALSXP, valuelist->size());
      for (int i = 0; i < length(av); i++)
        REAL(av)[i] = valuelist->at(i);
      m->setSexp(av);
      
      m->marshal();

      if (m->cleanup()) {
        delete valuelist;
      }
    }
    break;
  default:
    m->unsupported();
    break;
  }
}

void marshal_intVector(MethodCall *m) {
  switch(m->mode()) {
  case MethodCall::RToSmoke:
    {
      SEXP list = m->sexp();
      if (TYPEOF(list) != INTSXP) {
        m->item().s_voidp = 0;
        break;
      }
      int count = length(list);
      QVector<int> *valuelist = new QVector<int>;
      long i;
      for(i = 0; i < count; i++) {
        valuelist->append(INTEGER(list)[i]);
      }

      m->item().s_voidp = valuelist;
      m->marshal();
      
      if (!m->type().isConst()) {
        list = allocVector(INTSXP, count);
        for (int i = 0; i < count; i++)
          INTEGER(list)[i] = valuelist->at(i);
        m->setSexp(list);
      }

      if (m->cleanup()) {
        delete valuelist;
      }
    }
    break;
  case MethodCall::SmokeToR:
    {
      QVector<int> *valuelist = (QVector<int>*)m->item().s_voidp;
      if(!valuelist) {
        m->setSexp(R_NilValue);
        break;
      }

      SEXP av = allocVector(INTSXP, valuelist->size());
      for (int i = 0; i < length(av); i++)
        INTEGER(av)[i] = valuelist->at(i);
      m->setSexp(av);
      
      m->marshal();

      if (m->cleanup()) {
        delete valuelist;
      }
    }
    break;
  default:
    m->unsupported();
    break;
  }
}

void marshal_uintVector(MethodCall *m) {
  switch(m->mode()) {
  case MethodCall::RToSmoke:
    {
      SEXP list = m->sexp();
      if (TYPEOF(list) != REALSXP) {
        m->item().s_voidp = 0;
        break;
      }
      int count = length(list);
      QVector<unsigned int> *valuelist = new QVector<unsigned int>;
      long i;
      for(i = 0; i < count; i++) {
        valuelist->append(REAL(list)[i]);
      }

      m->item().s_voidp = valuelist;
      m->marshal();
      
      if (!m->type().isConst()) {
        list = allocVector(REALSXP, count);
        for (int i = 0; i < count; i++)
          REAL(list)[i] = valuelist->at(i);
        m->setSexp(list);
      }

      if (m->cleanup()) {
        delete valuelist;
      }
    }
    break;
  case MethodCall::SmokeToR:
    {
      QVector<unsigned int> *valuelist =
        (QVector<unsigned int>*)m->item().s_voidp;
      if(!valuelist) {
        m->setSexp(R_NilValue);
        break;
      }

      SEXP av = allocVector(REALSXP, valuelist->size());
      for (int i = 0; i < length(av); i++)
        REAL(av)[i] = valuelist->at(i);
      m->setSexp(av);
      
      m->marshal();

      if (m->cleanup()) {
        delete valuelist;
      }
    }
    break;
  default:
    m->unsupported();
    break;
  }
}

void marshal_voidP(MethodCall *m) {
  switch(m->mode()) {
  case MethodCall::RToSmoke:
    {
      SEXP rv = m->sexp();
      if (rv != R_NilValue)
        m->item().s_voidp = R_ExternalPtrAddr(m->sexp());
      else
        m->item().s_voidp = 0;
    }
    break;
  case MethodCall::SmokeToR:
    {
      m->setSexp(R_MakeExternalPtr(m->item().s_voidp, R_NilValue, R_NilValue));
    }
    break;
  default:
    m->unsupported();
    break;
  }
}

void marshal_QMapQStringQString(MethodCall *m) {
  switch(m->mode()) {
  case MethodCall::RToSmoke:
    {
      SEXP hash = m->sexp();
      if (TYPEOF(hash) != STRSXP) {
        m->item().s_voidp = 0;
        break;
      }
		
      QMap<QString,QString> * map = new QMap<QString,QString>;
      SEXP names = getAttrib(hash, R_NamesSymbol);
      for (long i = 0; i < length(hash); i++) {
        SEXP key = STRING_ELT(names, i);
        SEXP value = STRING_ELT(hash, i);
        (*map)[QString(CHAR(key))] = QString(CHAR(value));
      }
	    
      m->item().s_voidp = map;
      m->marshal();
		
      if(m->cleanup())
        delete map;
    }
    break;
  case MethodCall::SmokeToR:
    {
      QMap<QString,QString> *map = (QMap<QString,QString>*)m->item().s_voidp;
      if(!map) {
        m->setSexp(R_NilValue);
      }
		
      SEXP hv, names;
      PROTECT(hv = allocVector(STRSXP, map->size()));
      names = allocVector(STRSXP, length(hv));
      setAttrib(hv, R_NamesSymbol, names);
      
      QList<QString> keys = map->keys();
      QList<QString> values = map->values();
      for (int i = 0; i < length(hv); i++) {
        SET_STRING_ELT(names, i, qstring2sexp(keys[i]));
        SET_STRING_ELT(hv, i, qstring2sexp(values[i]));
      }

      UNPROTECT(1);
      
      m->setSexp(hv);
		
      if(m->cleanup())
        delete map;
    }
    break;
  default:
    m->unsupported();
    break;
  }
}

void marshal_QMapQStringQVariant(MethodCall *m) {
  switch(m->mode()) {
  case MethodCall::RToSmoke:
    {
      SEXP hash = m->sexp();
      if (TYPEOF(hash) != VECSXP) {
        m->item().s_voidp = 0;
        break;
      }
		
      QMap<QString,QVariant> * map = new QMap<QString,QVariant>;
      SEXP names = getAttrib(hash, R_NamesSymbol);
      
      for (long i = 0; i < length(hash); i++) {
        SEXP key = STRING_ELT(names, i);
        (*map)[QString(CHAR(key))] = asQVariant(VECTOR_ELT(hash, i));
      }
	    
      m->item().s_voidp = map;
      m->marshal();
      
      if(m->cleanup())
        delete map;
    }
    break;
  case MethodCall::SmokeToR:
    {
      QMap<QString,QVariant> *map = (QMap<QString,QVariant>*)m->item().s_voidp;
      if(!map) {
        m->setSexp(R_NilValue);
      }

      SEXP hv, names;
      PROTECT(hv = allocVector(VECSXP, map->size()));
      names = allocVector(STRSXP, length(hv));
      setAttrib(hv, R_NamesSymbol, names);
      
      QList<QString> keys = map->keys();
      QList<QVariant> values = map->values();
      for (int i = 0; i < length(hv); i++) {
        SET_STRING_ELT(names, i, qstring2sexp(keys[i]));
        SET_VECTOR_ELT(hv, i, asRVariant(values[i]));
      }

      UNPROTECT(1);

      m->setSexp(hv);
		
      if(m->cleanup())
        delete map;
    }
    break;
  default:
    m->unsupported();
    break;
  }
}

void marshal_QMapintQVariant(MethodCall *m) {
  switch(m->mode()) {
  case MethodCall::RToSmoke:
    {
      SEXP hash = m->sexp();
      if (TYPEOF(hash) != VECSXP) {
        m->item().s_voidp = 0;
        break;
      }
      
      QMap<int,QVariant> * map = new QMap<int,QVariant>;
      // a bit odd to store ints in 'names', but closest to a "map" in R
      SEXP names = getAttrib(hash, R_NamesSymbol);
      
      for (long i = 0; i < length(hash); i++) {
        SEXP key = STRING_ELT(names, i);
        (*map)[asInteger(key)] = asQVariant(VECTOR_ELT(hash, i));
      }
	    
      m->item().s_voidp = map;
      m->marshal();
      
      if(m->cleanup())
        delete map;
    }
    break;
  case MethodCall::SmokeToR:
    {
      QMap<int,QVariant> *map = (QMap<int,QVariant>*)m->item().s_voidp;
      if(!map) {
        m->setSexp(R_NilValue);
      }

      SEXP hash, names;
      PROTECT(hash = allocVector(VECSXP, map->size()));
      names = allocVector(STRSXP, length(hash));
      setAttrib(hash, R_NamesSymbol, names);
      
      QList<int> keys = map->keys();
      QList<QVariant> values = map->values();
      for (int i = 0; i < length(hash); i++) {
        SET_STRING_ELT(names, i, asChar(ScalarInteger(keys[i])));
        SET_VECTOR_ELT(hash, i, asRVariant(values[i]));
      }

      UNPROTECT(1);
      
      m->setSexp(hash);
		
      if(m->cleanup())
        delete map;
    }
    break;
  default:
    m->unsupported();
    break;
  }
}

void marshal_voidP_array(MethodCall *m) {
  switch(m->mode()) {
  case MethodCall::RToSmoke:
    {
      SEXP rv = m->sexp();
      if (rv != R_NilValue) {
        m->item().s_voidp = R_ExternalPtrAddr(rv);
      } else {
        m->item().s_voidp = 0;
      }
    }
    break;
  case MethodCall::SmokeToR:
    {
      SEXP rv = R_MakeExternalPtr(m->item().s_voidp, R_NilValue, R_NilValue);
      m->setSexp(rv);
    break;
    }
  default:
    m->unsupported();
    break;
  }
}

void marshal_QPairQStringQStringList(MethodCall *m) {
  switch(m->mode()) {
  case MethodCall::RToSmoke: 
    {
      SEXP list = m->sexp();
      if (TYPEOF(list) != STRSXP) {
        m->item().s_voidp = 0;
        break;
      }
      // Just treat this like a QMap<QString,QString>
      QList<QPair<QString,QString> > * pairlist =
        new QList<QPair<QString,QString> >();
      int count = length(list);
      SEXP names = getAttrib(list, R_NamesSymbol);
      for (long i = 0; i < count; i++) {
        SEXP s1 = STRING_ELT(names, i);
        SEXP s2 = STRING_ELT(list, i);
        QPair<QString,QString> * qpair =
          new QPair<QString,QString>(sexp2qstring(s1),sexp2qstring(s2));
        pairlist->append(*qpair);
      }

      m->item().s_voidp = pairlist;
      m->marshal();
			
      if (m->cleanup()) {
        delete pairlist;
      }
	   
      break;
    }

  case MethodCall::SmokeToR: 
    {
      QList<QPair<QString,QString> > *pairlist =
        static_cast<QList<QPair<QString,QString> > * >(m->item().s_voidp);
      if (pairlist == 0) {
        m->setSexp(R_NilValue);
      }

      SEXP hv, names;
      PROTECT(hv = allocVector(STRSXP, pairlist->size()));
      names = allocVector(STRSXP, length(hv));
      setAttrib(hv, R_NamesSymbol, names);
      
      for (int i = 0; i < length(hv); i++) {
        SET_STRING_ELT(names, i, qstring2sexp(pairlist->at(i).first));
        SET_STRING_ELT(hv, i, qstring2sexp(pairlist->at(i).second));
      }

      UNPROTECT(1);
      
      m->setSexp(hv);
      if (m->cleanup()) {
        delete pairlist;
      }

    }
    break;
  default:
    m->unsupported();
    break;
  }
}

void marshal_QPairdoubleQColorList(MethodCall *m) {
  switch(m->mode()) {
  case MethodCall::RToSmoke:
    {
      SEXP list = m->sexp();
      if (TYPEOF(list) != VECSXP) {
        m->item().s_voidp = 0;
        break;
      }
      QList<QPair<double,QColor> > * pairlist =
        new QList<QPair<double,QColor> >();
      for (int i = 0; i < length(list); i++) {
        SEXP pair = VECTOR_ELT(list, i);
        pairlist->append(QPair<double,QColor>(asReal(VECTOR_ELT(pair, 0)),
                                              asQColor(VECTOR_ELT(pair, 1))));
      }
      m->item().s_voidp = pairlist;
      m->marshal();
      if (m->cleanup()) {
        delete pairlist;
      }
    }
    break;
  case MethodCall::SmokeToR:
    {
      QList<QPair<double,QColor> > * qpairlist =
        static_cast<QList<QPair<double,QColor> >* >(m->item().s_voidp); 
      if (qpairlist == 0) {
        m->setSexp(R_NilValue);
      }

      SEXP av;
      PROTECT(av = allocVector(VECSXP, qpairlist->size()));
      for (int i = 0; i < length(av); i++) {
        SEXP pair = allocVector(VECSXP, 2);
        SET_VECTOR_ELT(av, i, pair);
        SET_VECTOR_ELT(pair, 0, ScalarReal(qpairlist->at(i).first));
        SET_VECTOR_ELT(pair, 1, asRColor(qpairlist->at(i).second));
      }
      
      m->setSexp(av);
      if (m->cleanup()) {
        //			delete qpair;
      }
      UNPROTECT(1);
    }
    break;
  default:
    m->unsupported();
    break;
  }
}

void marshal_QPairintint(MethodCall *m) {
  switch(m->mode()) {
  case MethodCall::RToSmoke:
    {
      SEXP pair = m->sexp();
      if (TYPEOF(pair) != INTSXP || length(pair) != 2) {
        m->item().s_voidp = 0;
        break;
      }
      int int0 = INTEGER(pair)[0];
      int int1 = INTEGER(pair)[1];
      		
      QPair<int,int> * qpair = new QPair<int,int>(int0,int1);
      m->item().s_voidp = qpair;
      m->marshal();

      if (m->cleanup()) {
        delete qpair;
      }
    }
    break;
  case MethodCall::SmokeToR:
  default:
    m->unsupported();
    break;
  }
}

DEF_LIST_MARSHALLER( QAbstractButtonList, QList<QAbstractButton*>, QAbstractButton )
DEF_LIST_MARSHALLER( QActionGroupList, QList<QActionGroup*>, QActionGroup )
DEF_LIST_MARSHALLER( QActionList, QList<QAction*>, QAction )
DEF_LIST_MARSHALLER( QListWidgetItemList, QList<QListWidgetItem*>, QListWidgetItem )
DEF_LIST_MARSHALLER( QObjectList, QList<QObject*>, QObject )
DEF_LIST_MARSHALLER( QTableWidgetList, QList<QTableWidget*>, QTableWidget )
DEF_LIST_MARSHALLER( QTableWidgetItemList, QList<QTableWidgetItem*>, QTableWidgetItem )
DEF_LIST_MARSHALLER( QTextFrameList, QList<QTextFrame*>, QTextFrame )
DEF_LIST_MARSHALLER( QTreeWidgetItemList, QList<QTreeWidgetItem*>, QTreeWidgetItem )
DEF_LIST_MARSHALLER( QTreeWidgetList, QList<QTreeWidget*>, QTreeWidget )
DEF_LIST_MARSHALLER( QWidgetList, QList<QWidget*>, QWidget )

#if QT_VERSION >= 0x40200
DEF_LIST_MARSHALLER( QGraphicsItemList, QList<QGraphicsItem*>, QGraphicsItem )
DEF_LIST_MARSHALLER( QGraphicsViewList, QList<QGraphicsView*>, QGraphicsView )
DEF_LIST_MARSHALLER( QStandardItemList, QList<QStandardItem*>, QStandardItem )
DEF_LIST_MARSHALLER( QUndoStackList, QList<QUndoStack*>, QUndoStack )
#endif

#if QT_VERSION >= 0x40300
DEF_LIST_MARSHALLER( QMdiSubWindowList, QList<QMdiSubWindow*>, QMdiSubWindow )
#endif

DEF_VALUELIST_MARSHALLER( QColorVector, QVector<QColor>, QColor )
DEF_VALUELIST_MARSHALLER( QFileInfoList, QFileInfoList, QFileInfo )
#ifdef QT_QTNETWORK
DEF_VALUELIST_MARSHALLER( QHostAddressList, QList<QHostAddress>, QHostAddress )
#endif
DEF_VALUELIST_MARSHALLER( QImageTextKeyLangList, QList<QImageTextKeyLang>, QImageTextKeyLang )
DEF_VALUELIST_MARSHALLER( QKeySequenceList, QList<QKeySequence>, QKeySequence )
DEF_VALUELIST_MARSHALLER( QLineFVector, QVector<QLineF>, QLineF )
DEF_VALUELIST_MARSHALLER( QLineVector, QVector<QLine>, QLine )
DEF_VALUELIST_MARSHALLER( QModelIndexList, QList<QModelIndex>, QModelIndex )
#ifdef QT_QTNETWORK
DEF_VALUELIST_MARSHALLER( QNetworkAddressEntryList, QList<QNetworkAddressEntry>, QNetworkAddressEntry )
DEF_VALUELIST_MARSHALLER( QNetworkInterfaceList, QList<QNetworkInterface>, QNetworkInterface )
#endif
DEF_VALUELIST_MARSHALLER( QPixmapList, QList<QPixmap>, QPixmap )
DEF_VALUELIST_MARSHALLER( QPointFVector, QVector<QPointF>, QPointF )
DEF_VALUELIST_MARSHALLER( QPointVector, QVector<QPoint>, QPoint )
DEF_VALUELIST_MARSHALLER( QPolygonFList, QList<QPolygonF>, QPolygonF )
DEF_VALUELIST_MARSHALLER( QRectFList, QList<QRectF>, QRectF )
DEF_VALUELIST_MARSHALLER( QRectFVector, QVector<QRectF>, QRectF )
DEF_VALUELIST_MARSHALLER( QRectVector, QVector<QRect>, QRect )
DEF_VALUELIST_MARSHALLER( QRgbVector, QVector<QRgb>, QRgb )
DEF_VALUELIST_MARSHALLER( QTableWidgetSelectionRangeList, QList<QTableWidgetSelectionRange>, QTableWidgetSelectionRange )
DEF_VALUELIST_MARSHALLER( QTextBlockList, QList<QTextBlock>, QTextBlock )
DEF_VALUELIST_MARSHALLER( QTextEditExtraSelectionsList, QList<QTextEdit::ExtraSelection>, QTextEdit::ExtraSelection )
DEF_VALUELIST_MARSHALLER( QTextFormatVector, QVector<QTextFormat>, QTextFormat )
DEF_VALUELIST_MARSHALLER( QTextLayoutFormatRangeList, QList<QTextLayout::FormatRange>, QTextLayout::FormatRange)
DEF_VALUELIST_MARSHALLER( QTextLengthVector, QVector<QTextLength>, QTextLength )
DEF_VALUELIST_MARSHALLER( QSizeFList, QList<QSizeF>, QSizeF )
DEF_VALUELIST_MARSHALLER( QUrlList, QList<QUrl>, QUrl )
DEF_VALUELIST_MARSHALLER( QVariantList, QList<QVariant>, QVariant )
DEF_VALUELIST_MARSHALLER( QVariantVector, QVector<QVariant>, QVariant )

#if QT_VERSION >= 0x40300
#ifdef QT_QTNETWORK
DEF_VALUELIST_MARSHALLER( QSslCertificateList, QList<QSslCertificate>, QSslCertificate )
DEF_VALUELIST_MARSHALLER( QSslCipherList, QList<QSslCipher>, QSslCipher )
DEF_VALUELIST_MARSHALLER( QSslErrorList, QList<QSslError>, QSslError )
#endif
#ifdef QT_QTXML
DEF_VALUELIST_MARSHALLER( QXmlStreamEntityDeclarations, QVector<QXmlStreamEntityDeclaration>, QXmlStreamEntityDeclaration )
DEF_VALUELIST_MARSHALLER( QXmlStreamNamespaceDeclarations, QVector<QXmlStreamNamespaceDeclaration>, QXmlStreamNamespaceDeclaration )
DEF_VALUELIST_MARSHALLER( QXmlStreamNotationDeclarations, QVector<QXmlStreamNotationDeclaration>, QXmlStreamNotationDeclaration )
#endif
#endif

#if QT_VERSION >= 0x40400
DEF_LIST_MARSHALLER( QGraphicsWidgetList, QList<QGraphicsWidget*>,
                     QGraphicsWidget )
#ifdef QT_QTNETWORK
DEF_VALUELIST_MARSHALLER( QNetworkCookieList, QList<QNetworkCookie>, QNetworkCookie )
#endif
DEF_VALUELIST_MARSHALLER( QPrinterInfoList, QList<QPrinterInfo>, QPrinterInfo )
#endif

#if QT_VERSION >= 0x40600
DEF_LIST_MARSHALLER( QAbstractAnimationList, QList<QAbstractAnimation*>, QAbstractAnimation )
DEF_LIST_MARSHALLER( QAbstractStateList, QList<QAbstractState*>, QAbstractState )
DEF_LIST_MARSHALLER( QDockWidgetList, QList<QDockWidget*>, QDockWidget )
DEF_LIST_MARSHALLER( QGraphicsTransformList, QList<QGraphicsTransform*>,
                     QGraphicsTransform )
#ifdef QT_QTOPENGL
DEF_LIST_MARSHALLER( QGLShaderList, QList<QGLShader*>, QGLShader )
#endif
#endif

/* Define marshallers for value types */
DEF_VALUE_MARSHALLER_SCORER(RectF)
DEF_VALUE_MARSHALLER_SCORER(ByteArray)

Q_DECL_EXPORT TypeHandler Qt_handlers[] = {
  { "void", marshal_void, NULL },
  { "bool*", marshal_it<bool *>, NULL },
  { "bool&", marshal_it<bool *>, NULL },
  { "char**", marshal_charP_array, NULL },
  { "char*", marshal_it<char *>, NULL },
  { "const char*", marshal_it<char *>, NULL },
  { "const char* const *", marshal_it<char *>, NULL },
  // TODO: integrate with char** above, this is a reference to single string
  //{ "char*&", marshal_it<char **>, NULL },
  { "char&", marshal_it<char *>, NULL },
  { "signed char&", marshal_it<char *>, NULL },
  { "double*", marshal_doubleR, NULL },
  { "double&", marshal_doubleR, NULL },
  // TODO { "float&", marshal_it<float *>, NULL },
  { "int*", marshal_it<int *>, NULL },
  { "const int*", marshal_it<int *>, NULL },
  { "int&", marshal_it<int *>, NULL },
  { "signed int&", marshal_it<int *>, NULL },
  // BUG: it looks like Smoke marks "long long" as a long, WRONG
  // TODO { "long*", marshal_it<long *>, NULL },
  // TODO { "signed long&", marshal_it<long *>, NULL },
  // TODO { "unsigned long&", marshal_it<long *>, NULL },
  // TODO { "short&", marshal_it<short *>, NULL },
  // TODO { "signed short&", marshal_it<short *>, NULL },
  // TODO { "unsigned short&", marshal_it<short *>, NULL },
  { "long long", marshal_it<long long>, NULL },
  { "long long&", marshal_it<long long>, NULL },
  { "unsigned long long&", marshal_it<long long>, NULL },
  { "long long*", marshal_it<long long>, NULL },
  { "unsigned int&", marshal_it<unsigned int *>, NULL },
  { "const unsigned int*", marshal_it<unsigned int *>, NULL },
  { "signed int&", marshal_it<int *>, NULL },
  { "unsigned char*", marshal_it<unsigned char *>, NULL },
  // TODO { "unsigned char&", marshal_it<unsigned char *>, NULL },
  { "void**", marshal_voidP_array, NULL },
  { "FILE*", marshal_voidP, NULL },
  { "const void*", marshal_voidP, NULL },
  { "size_t", marshal_it<unsigned int>, NULL },
  { "GLenum", marshal_it<int *>, NULL },
  // TODO: { "GLfloat", marshal_it<float *>, NULL },
  { "GLint", marshal_it<int *>, NULL },
  { "GLuint", marshal_it<unsigned int *>, NULL },
  { "GLbitfield", marshal_it<unsigned int *>, NULL },
  { "QList<const char*>", marshal_CharStarList, NULL },
  { "const QString", marshal_QString, NULL },
#ifdef QT_QTDBUS
  TYPE_HANDLER_ENTRY_VALUE(QDBusVariant),
#endif
  TYPE_HANDLER_ENTRY_PAIRLIST(double, QColor),
  // TODO:  TYPE_HANDLER_ENTRY_PAIRVECTOR(double, QColor),
  // TODO:  TYPE_HANDLER_ENTRY_PAIRVECTOR(double, QVariant),
  TYPE_HANDLER_ENTRY_VALUELIST(int),
  TYPE_HANDLER_ENTRY_VALUELIST(uint),
  TYPE_HANDLER_ENTRY_LIST(QAbstractButton),
  TYPE_HANDLER_ENTRY_LIST(QAction),
  TYPE_HANDLER_ENTRY_VALUELIST(QByteArray),
  TYPE_HANDLER_ENTRY_VALUELIST(QFileInfo),
  TYPE_HANDLER_ENTRY_VALUELIST(QFileInfo),
#ifdef QT_QTOPENGL
  TYPE_HANDLER_ENTRY_LIST(QGLShader),
#endif    
#ifdef QT_QTNETWORK
  TYPE_HANDLER_ENTRY_VALUELIST(QHostAddress),
#endif
  TYPE_HANDLER_ENTRY_VALUELIST(QImageTextKeyLang),
  TYPE_HANDLER_ENTRY_VALUELIST(QKeySequence),
  TYPE_HANDLER_ENTRY_VALUELIST(QModelIndex),
  TYPE_HANDLER_ENTRY_LIST(QListWidgetItem),
#ifdef QT_QTNETWORK
  TYPE_HANDLER_ENTRY_VALUELIST(QNetworkAddressEntry),
  TYPE_HANDLER_ENTRY_VALUELIST(QNetworkInterface),
#endif
  TYPE_HANDLER_ENTRY_PAIRLIST(QString, QString),
  //TODO:TYPE_HANDLER_ENTRY_PAIRLIST(QByteArray, QByteArray),
  //TODO:TYPE_HANDLER_ENTRY_PAIRLIST(double, QPointF),
  //TODO:TYPE_HANDLER_ENTRY_PAIRLIST(double, double),
  //TODO:TYPE_HANDLER_ENTRY_PAIRLIST(int, int),
  TYPE_HANDLER_ENTRY_LIST(QPolygonF),
  TYPE_HANDLER_ENTRY_VALUELIST(QRectF),
  TYPE_HANDLER_ENTRY_VALUELIST(QSizeF),
  TYPE_HANDLER_ENTRY_VALUELIST(double),
  TYPE_HANDLER_ENTRY_LIST(QObject),
  TYPE_HANDLER_ENTRY_LIST(QTableWidgetItem),
  TYPE_HANDLER_ENTRY_VALUELIST(QTableWidgetSelectionRange),
  TYPE_HANDLER_ENTRY_VALUELIST(QTextBlock),
  /* FIXME: QLists of enums should not become R lists of externalptrs
  { "QList<QTextEdit::ExtraSelection>", marshal_QTextEditExtraSelectionsList, NULL },
  { "QList<QTextEdit::ExtraSelection>&", marshal_QTextEditExtraSelectionsList, NULL },
  TYPE_HANDLER_ENTRY_LIST(QTextFrame),
  { "QList<QTextLayout::FormatRange>", marshal_QTextLayoutFormatRangeList, NULL },
  { "QList<QTextLayout::FormatRange>&",
  marshal_QTextLayoutFormatRangeList, NULL },
  
  Additional lists of enums:
  QInputMethodEvent::Attribute,
  QTextOption::Tab,
  QWizard::WizardButton,
  QFontDatabase::WritingSystem, 
  QLocale::Country,
  QPrinter::PageSize,
  QAbstractTextDocumentLayout::Selection,
  And vectors: QAbstractTextDocumentLayout::Selection, QTextLayout::FormatRange
  */
  TYPE_HANDLER_ENTRY_LIST(QTreeWidgetItem),
  TYPE_HANDLER_ENTRY_LIST(QUndoStack),
  TYPE_HANDLER_ENTRY_VALUELIST(QUrl),
  TYPE_HANDLER_ENTRY_VALUELIST(QVariant),
  TYPE_HANDLER_ENTRY_LIST(QWidget),
  TYPE_HANDLER_ENTRY_MAP(int, QVariant),
  TYPE_HANDLER_ENTRY_MAP(QString, QVariant),
  // FIXME: need QHash variant for the above, and int->QByteArray
  TYPE_HANDLER_ENTRY_MAP(QString, QString),
  // FIXME: need QMap marshaller for QDate->QTextCharFormat
  TYPE_HANDLER_ENTRY_PAIR(int, int),
  TYPE_HANDLER_ENTRY_VALUE(QStringList),
  TYPE_HANDLER_ENTRY_VALUE(QString),
  TYPE_HANDLER_ENTRY_VALUEVECTOR(QColor),
  TYPE_HANDLER_ENTRY_VALUEVECTOR(QLine),
  TYPE_HANDLER_ENTRY_VALUEVECTOR(QLineF),
  TYPE_HANDLER_ENTRY_VALUEVECTOR(QPointF),
  TYPE_HANDLER_ENTRY_VALUEVECTOR(QPoint),
  TYPE_HANDLER_ENTRY_VALUEVECTOR(int),
  TYPE_HANDLER_ENTRY_VALUEVECTOR(QRectF),
  TYPE_HANDLER_ENTRY_VALUEVECTOR(QRect),
  TYPE_HANDLER_ENTRY_VALUEVECTOR(QTextFormat),
  TYPE_HANDLER_ENTRY_VALUEVECTOR(QTextLength),
  TYPE_HANDLER_ENTRY_VALUEVECTOR(QVariant),
  TYPE_HANDLER_ENTRY_VALUEVECTOR(double),
  // TODO: types with whitespace are an issue, but not after moving to templates
  //TYPE_HANDLER_ENTRY_VECTOR_FULL(unsigned int, uint),
#if QT_VERSION >= 0x40200
  TYPE_HANDLER_ENTRY_LIST(QGraphicsItem),
  TYPE_HANDLER_ENTRY_LIST(QGraphicsView),
  TYPE_HANDLER_ENTRY_LIST(QGraphicsWidget),
  TYPE_HANDLER_ENTRY_LIST(QStandardItem),
#endif
#if QT_VERSION >= 0x40300
  TYPE_HANDLER_ENTRY_LIST(QMdiSubWindow),
#ifdef QT_QTNETWORK
  TYPE_HANDLER_ENTRY_VALUELIST(QSslCertificate),
  TYPE_HANDLER_ENTRY_VALUELIST(QSslCipher),
  TYPE_HANDLER_ENTRY_VALUELIST(QSslError),
#endif
#ifdef QT_QTXML
  TYPE_HANDLER_ENTRY_VALUEVECTOR(QXmlStreamEntityDeclaration),
  TYPE_HANDLER_ENTRY_VALUEVECTOR(QXmlStreamNamespaceDeclaration),
  TYPE_HANDLER_ENTRY_VALUEVECTOR(QXmlStreamNotationDeclaration),
#endif
#endif
#if QT_VERSION >= 0x040400
#ifdef QT_QTNETWORK
  TYPE_HANDLER_ENTRY_VALUELIST(QNetworkCookie),
#endif
  TYPE_HANDLER_ENTRY_VALUELIST(QPrinterInfo),
#endif
#if QT_VERSION >= 0x40600
  TYPE_HANDLER_ENTRY_LIST(QAbstractAnimation),
  TYPE_HANDLER_ENTRY_LIST(QAbstractState),
  TYPE_HANDLER_ENTRY_LIST(QDockWidget),
  TYPE_HANDLER_ENTRY_LIST(QGraphicsTransform),
#endif
  TYPE_HANDLER_ENTRY_VALUE(QRectF),
  /* TODO: support more of these
  TYPE_HANDLER_ENTRY(QSizeF),
  TYPE_HANDLER_ENTRY(QPointF),
  TYPE_HANDLER_ENTRY(QSize),
  TYPE_HANDLER_ENTRY(QPoint),
  TYPE_HANDLER_ENTRY(QRect),
  TYPE_HANDLER_ENTRY(QTransform),
  TYPE_HANDLER_ENTRY(QColor),
  TYPE_HANDLER_ENTRY(QVariant),
  TYPE_HANDLER_ENTRY(QUrl),
  */
  /* TODO: implement marshal_QGenericMatrix<N,M,T>
  TYPE_HANDLER_ENTRY_MATRIX(2, 2, double),
  TYPE_HANDLER_ENTRY_MATRIX(2, 3, double),
  TYPE_HANDLER_ENTRY_MATRIX(2, 4, double),
  TYPE_HANDLER_ENTRY_MATRIX(3, 2, double),
  TYPE_HANDLER_ENTRY_MATRIX(3, 2, double),
  TYPE_HANDLER_ENTRY_MATRIX(3, 3, double),
  TYPE_HANDLER_ENTRY_MATRIX(3, 4, double),
  TYPE_HANDLER_ENTRY_MATRIX(4, 2, double),
  TYPE_HANDLER_ENTRY_MATRIX(4, 3, double),
  */
  // FIXME: need QSet<QAccessible::Method>
  { 0, 0, NULL }
};

void init_type_handlers() {
  // FIXME: do we want to integrate this with RQtModule?
  MethodCall::registerTypeHandlers(Qt_handlers);
}
