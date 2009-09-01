/* Largely adapted from QtRuby */

/* Type conversion thoughts:

   See types.txt for a list of types we need to handle.

   Should probably factor out the actual conversions implemented in
   this file, so that we can use them from the QVariant converter.
   
   Enums: It does not seem like Smoke provides the string name of enum
   values. Qt does not help much either, because QMetaEnum only works
   for enums defined in a class. Many are not, and how would we get
   the class anyway?

   Values: Many value types (e.g. QRectF) can be handled through
   high-level type conversion. Need a macro for this. Converters that
   we currently lack:
     QBrush/QPen: something in qtgui, needs to be expanded
     QByteArray: below, but needs to be standardized
     QKeySequence: from string or standard key (integer)
     QUrl/QHostAddress: from strings
     QPolygon(F)/QLine(F): from 2 column matrix
     QTransform: like QMatrix is now

     On an as-needed basis:
     QDBusSignature/QLatin1String: from string
     GLenum/GLuint/QRgb: from integer
     QDate/QTime: from R date and time objects
     QStyleOption (and subclasses): from an R list
     QTextFormat/QPalette/QTextOption: similar to QBrush/QPen
     QBitArray: from an R raw vector
     QNetworkCacheMetaData/QUrlInfo: R list?
     QSslKey/QSslCipher: as character + attributes for other fields?
     QXmlStreamAttribute: as character + attributes
     QTextLength: as integer + plus type attribute
     QUuid, QXmlStream*Declaration: as string
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
     QTableWidgetSelectionRange: like QRect?
     
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
   - Conditional compilation of OpenGL types
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
#include <QtNetwork/qhostaddress.h>
#include <QtNetwork/qnetworkinterface.h>
#include <QtNetwork/qurlinfo.h>


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
#include <QtNetwork/qsslcertificate.h>
#include <QtNetwork/qsslcipher.h>
#include <QtNetwork/qsslerror.h>
#include <QtXml/qxmlstream.h>
#endif

#if QT_VERSION >= 0x040400
#include <QtGui/qprinterinfo.h>
#include <QtNetwork/qnetworkcookie.h>
#endif

#include <smoke.h>
#include "type-handlers.hpp"
#include "convert.hpp"
#undef isNull // R causing trouble again

#ifndef HINT_BYTES
#define HINT_BYTES HINT_BYTE
#endif

bool
matches_arg(Smoke *smoke, Smoke::Index meth, Smoke::Index argidx,
            const char *argtype)
{
  Smoke::Index *arg = smoke->argumentList + smoke->methods[meth].args + argidx;
  SmokeType type = SmokeType(smoke, *arg);
  if (type.name() && qstrcmp(type.name(), argtype) == 0) {
    return true;
  }
  return false;
}

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

void
marshal_basetype(MethodCall *m)
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
  QByteArray className =
    QByteArray(CHAR(asChar(getAttrib(value, R_ClassSymbol))));
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
      if (inherits(value, "QtEnum") && className == type.name())
        score = 3;
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
      score = 2;
      break;
    case Smoke::t_int:
    case Smoke::t_uint:
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
  case LGLSXP:
    if (elem == Smoke::t_bool)
      score = 3;
    break;
  case STRSXP:
    if (elem == Smoke::t_char && strlen(CHAR(asChar(value))) == 1)
      score = 2;
    break;
  case EXTPTRSXP:
    if (elem == Smoke::t_class) {
      SmokeObject *o = SmokeObject::fromExternalPtr(value);
      if (!o || !o->smoke()) // some kind of foreign pointer
        score = 1;
      else {
        const char *smokeClass = o->smoke()->classes[o->classId()].className;
        if (className == smokeClass)
          score = 3;
        else if (o->instanceOf(className))
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
void scoreArg_unknown(SEXP /*arg*/, SmokeType type) {
  error("unable to score argument of type '%s'", type.name());
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
      if (m->cleanup() && m->type().isConst()) {
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

QByteArray*
qbytearrayFromRString(SEXP rstring) {
  return new QByteArray(CHAR(asChar(rstring)));
}

SEXP
rstringFromQByteArray(QByteArray * s) {
  return mkString(s->data());
}

static void marshal_QString(MethodCall *m) {
  switch(m->mode()) {
  case MethodCall::RToSmoke:
    { // MFL: changed 's' to static allocation
      QString s;
      if( m->sexp() != R_NilValue) {
        s = sexp2qstring(m->sexp());
      }
      
      m->item().s_voidp = &s;
      m->marshal();

      // FIXME: shouldn't we check that we have a pointer/ref? 
      if (!m->type().isConst() && m->sexp() != R_NilValue && !s.isNull())
      { 
        m->setSexp(qstring2sexp(s));
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
        if(m->cleanup() || m->type().isStack() ) {
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

static void marshal_QByteArray(MethodCall *m) {
  switch(m->mode()) {
  case MethodCall::RToSmoke:
    {
      QByteArray* s = 0;
      if( m->sexp() != R_NilValue) {
        s = qbytearrayFromRString(m->sexp());
      } else {
        s = new QByteArray();
      }

      m->item().s_voidp = s;
      m->marshal();

      if (!m->type().isConst() && m->sexp() != R_NilValue && s != 0 &&
          !s->isNull())
      {
        m->setSexp(rstringFromQByteArray(s));
      }

      if (s != 0 && m->cleanup()) {
        delete s;
      }
    }
    break;

  case MethodCall::SmokeToR:
    {
      QByteArray *s = (QByteArray*)m->item().s_voidp;
      if(s) {
        if (s->isNull()) {
          m->setSexp(R_NilValue);
        } else {
          m->setSexp(rstringFromQByteArray(s));
        }
        if(m->cleanup() || m->type().isStack() ) {
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

void marshal_QDBusVariant(MethodCall *m) {
  switch(m->mode()) {
  case MethodCall::RToSmoke: 
    {
      SEXP v = m->sexp();
      if (v == R_NilValue) {
        m->item().s_voidp = 0;
        break;
      }

      SmokeObject *o = SmokeObject::fromExternalPtr(v);
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
                             m->type().isStack())->externalPtr();

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

void marshal_QListCharStar(MethodCall *m) {
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

void marshal_QListInt(MethodCall *m) {
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


void marshal_QListUInt(MethodCall *m) {
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

void marshal_QListqreal(MethodCall *m) {
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

void marshal_QVectorqreal(MethodCall *m) {
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

void marshal_QVectorint(MethodCall *m) {
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

void marshal_QMapIntQVariant(MethodCall *m) {
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

void marshal_QRgb_array(MethodCall *m) {
  switch(m->mode()) {
  case MethodCall::RToSmoke:
    {
      SEXP list = m->sexp();
      if (TYPEOF(list) != INTSXP) {
        m->item().s_voidp = 0;
        break;
      }
      int count = length(list);
      QRgb *rgb = new QRgb[count + 2];
      int i, *rrgb = INTEGER(list);
      for(i = 0; i < count; i++, rrgb += 3)
        rgb[i] = (rrgb[0] << 16) + (rrgb[1] << 8) + rrgb[2];
      m->item().s_voidp = rgb;
      m->marshal();
    }
    break;
  case MethodCall::SmokeToR:
    // TODO?
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

void marshal_QPairqrealQColor(MethodCall *m) {
  switch(m->mode()) {
  case MethodCall::RToSmoke:
    {
      SEXP list = m->sexp();
      if (TYPEOF(list) != VECSXP || length(list) != 2) {
        m->item().s_voidp = 0;
        break;
      }

      qreal real;
      SEXP item1 = VECTOR_ELT(list, 0);
      if (TYPEOF(item1) != REALSXP) {
        real = 0;
      } else {
        real = asReal(item1);
      }
		
      SEXP item2 = VECTOR_ELT(list, 1);
      		
      QPair<qreal,QColor> * qpair =
        new QPair<qreal,QColor>(real, asQColor(item2));
      m->item().s_voidp = qpair;
      m->marshal();

      if (m->cleanup()) {
        delete qpair;
      }
    }
    break;
  case MethodCall::SmokeToR:
    {
      QPair<qreal,QColor> * qpair =
        static_cast<QPair<qreal,QColor> * >(m->item().s_voidp); 
      if (qpair == 0) {
        m->setSexp(R_NilValue);
      }

      SEXP av;
      PROTECT(av = allocVector(VECSXP, 2));
      SET_VECTOR_ELT(av, 0, ScalarReal(qpair->first));
      SET_VECTOR_ELT(av, 1, asRColor(qpair->second));
      UNPROTECT(1);
      
      m->setSexp(av);
      if (m->cleanup()) {
        //			delete qpair;
      }
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
DEF_LIST_MARSHALLER( QWidgetPtrList, QList<QWidget*>, QWidget )

#if QT_VERSION >= 0x40200
DEF_LIST_MARSHALLER( QGraphicsItemList, QList<QGraphicsItem*>, QGraphicsItem )
DEF_LIST_MARSHALLER( QStandardItemList, QList<QStandardItem*>, QStandardItem )
DEF_LIST_MARSHALLER( QUndoStackList, QList<QUndoStack*>, QUndoStack )
#endif

#if QT_VERSION >= 0x40300
DEF_LIST_MARSHALLER( QMdiSubWindowList, QList<QMdiSubWindow*>, QMdiSubWindow )
#endif

DEF_VALUELIST_MARSHALLER( QColorVector, QVector<QColor>, QColor )
DEF_VALUELIST_MARSHALLER( QFileInfoList, QFileInfoList, QFileInfo )
DEF_VALUELIST_MARSHALLER( QHostAddressList, QList<QHostAddress>, QHostAddress )
DEF_VALUELIST_MARSHALLER( QImageTextKeyLangList, QList<QImageTextKeyLang>, QImageTextKeyLang )
DEF_VALUELIST_MARSHALLER( QKeySequenceList, QList<QKeySequence>, QKeySequence )
DEF_VALUELIST_MARSHALLER( QLineFVector, QVector<QLineF>, QLineF )
DEF_VALUELIST_MARSHALLER( QLineVector, QVector<QLine>, QLine )
DEF_VALUELIST_MARSHALLER( QModelIndexList, QList<QModelIndex>, QModelIndex )
DEF_VALUELIST_MARSHALLER( QNetworkAddressEntryList, QList<QNetworkAddressEntry>, QNetworkAddressEntry )
DEF_VALUELIST_MARSHALLER( QNetworkInterfaceList, QList<QNetworkInterface>, QNetworkInterface )
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
DEF_VALUELIST_MARSHALLER( QUrlList, QList<QUrl>, QUrl )
DEF_VALUELIST_MARSHALLER( QVariantList, QList<QVariant>, QVariant )
DEF_VALUELIST_MARSHALLER( QVariantVector, QVector<QVariant>, QVariant )

#if QT_VERSION >= 0x40300
DEF_VALUELIST_MARSHALLER( QSslCertificateList, QList<QSslCertificate>, QSslCertificate )
DEF_VALUELIST_MARSHALLER( QSslCipherList, QList<QSslCipher>, QSslCipher )
DEF_VALUELIST_MARSHALLER( QSslErrorList, QList<QSslError>, QSslError )
DEF_VALUELIST_MARSHALLER( QXmlStreamEntityDeclarations, QVector<QXmlStreamEntityDeclaration>, QXmlStreamEntityDeclaration )
DEF_VALUELIST_MARSHALLER( QXmlStreamNamespaceDeclarations, QVector<QXmlStreamNamespaceDeclaration>, QXmlStreamNamespaceDeclaration )
DEF_VALUELIST_MARSHALLER( QXmlStreamNotationDeclarations, QVector<QXmlStreamNotationDeclaration>, QXmlStreamNotationDeclaration )
#endif

#if QT_VERSION >= 0x40400
DEF_VALUELIST_MARSHALLER( QNetworkCookieList, QList<QNetworkCookie>, QNetworkCookie )
DEF_VALUELIST_MARSHALLER( QPrinterInfoList, QList<QPrinterInfo>, QPrinterInfo )
#endif

Q_DECL_EXPORT TypeHandler Qt_handlers[] = {
  { "bool*", marshal_it<bool *>, NULL },
  { "bool&", marshal_it<bool *>, NULL },
  { "char**", marshal_charP_array, NULL },
  { "const char*", marshal_it<const char *>, NULL },
  //{ "char*", marshal_it<char *>, NULL },
  { "DOM::DOMTimeStamp", marshal_it<long long>, NULL },
  { "double*", marshal_doubleR, NULL },
  { "double&", marshal_doubleR, NULL },
  { "int*", marshal_it<int *>, NULL },
  { "int&", marshal_it<int *>, NULL },
  { "KIO::filesize_t", marshal_it<long long>, NULL },
  { "long long int", marshal_it<long long>, NULL },
  { "long long int&", marshal_it<long long>, NULL },
  { "QDBusVariant", marshal_QDBusVariant, NULL },
  { "QDBusVariant&", marshal_QDBusVariant, NULL },
  { "QList<QFileInfo>", marshal_QFileInfoList, NULL },
  { "QFileInfoList", marshal_QFileInfoList, NULL },
  { "QGradiantStops", marshal_QPairqrealQColor, NULL },
  { "QGradiantStops&", marshal_QPairqrealQColor, NULL },
  { "unsigned int&", marshal_it<unsigned int *>, NULL },
  { "quint32&", marshal_it<unsigned int *>, NULL },
  { "uint&", marshal_it<unsigned int *>, NULL },
  { "qint32&", marshal_it<int *>, NULL },
  { "qint64", marshal_it<long long>, NULL },
  { "qint64&", marshal_it<long long>, NULL },
  { "QList<const char*>", marshal_QListCharStar, NULL },
  { "QList<int>", marshal_QListInt, NULL },
  { "QList<int>&", marshal_QListInt, NULL },
  { "QList<uint>", marshal_QListUInt, NULL },
  { "QList<uint>&", marshal_QListUInt, NULL },
  { "QList<QAbstractButton*>", marshal_QAbstractButtonList, NULL },
  { "QList<QActionGroup*>", marshal_QActionGroupList, NULL },
  { "QList<QAction*>", marshal_QActionList, NULL },
  { "QList<QAction*>&", marshal_QActionList, NULL },
  { "QList<QByteArray>", marshal_QByteArrayList, NULL },
  { "QList<QByteArray>*", marshal_QByteArrayList, NULL },
  { "QList<QByteArray>&", marshal_QByteArrayList, NULL },
  { "QList<QHostAddress>", marshal_QHostAddressList, NULL },
  { "QList<QHostAddress>&", marshal_QHostAddressList, NULL },
  { "QList<QImageTextKeyLang>", marshal_QImageTextKeyLangList, NULL },
  { "QList<QKeySequence>", marshal_QKeySequenceList, NULL },
  { "QList<QKeySequence>&", marshal_QKeySequenceList, NULL },
  { "QList<QListWidgetItem*>", marshal_QListWidgetItemList, NULL },
  { "QList<QListWidgetItem*>&", marshal_QListWidgetItemList, NULL },
  { "QList<QModelIndex>", marshal_QModelIndexList, NULL },
  { "QList<QModelIndex>&", marshal_QModelIndexList, NULL },
  { "QList<QNetworkAddressEntry>", marshal_QNetworkAddressEntryList, NULL },
  { "QList<QNetworkInterface>", marshal_QNetworkInterfaceList, NULL },
  { "QList<QPair<QString,QString> >", marshal_QPairQStringQStringList, NULL },
  { "QList<QPair<QString,QString> >&", marshal_QPairQStringQStringList, NULL },
  { "QList<QPixmap>", marshal_QPixmapList, NULL },
  { "QList<QPolygonF>", marshal_QPolygonFList, NULL },
  { "QList<QRectF>", marshal_QRectFList, NULL },
  { "QList<QRectF>&", marshal_QRectFList, NULL },
  { "QList<qreal>", marshal_QListqreal, NULL },
  { "QList<double>", marshal_QListqreal, NULL },
  { "QwtValueList", marshal_QListqreal, NULL },
  { "QwtValueList&", marshal_QListqreal, NULL },
  { "QList<double>&", marshal_QListqreal, NULL },
  { "QList<QObject*>", marshal_QObjectList, NULL },
  { "QList<QObject*>&", marshal_QObjectList, NULL },
  { "QList<QTableWidgetItem*>", marshal_QTableWidgetItemList, NULL },
  { "QList<QTableWidgetItem*>&", marshal_QTableWidgetItemList, NULL },
  { "QList<QTableWidgetSelectionRange>", marshal_QTableWidgetSelectionRangeList, NULL },
  { "QList<QTextBlock>", marshal_QTextBlockList, NULL },
  { "QList<QTextEdit::ExtraSelection>", marshal_QTextEditExtraSelectionsList, NULL },
  { "QList<QTextEdit::ExtraSelection>&", marshal_QTextEditExtraSelectionsList, NULL },
  { "QList<QTextFrame*>", marshal_QTextFrameList, NULL },
  { "QList<QTextLayout::FormatRange>", marshal_QTextLayoutFormatRangeList, NULL },
  { "QList<QTextLayout::FormatRange>&", marshal_QTextLayoutFormatRangeList, NULL },
  { "QList<QTreeWidgetItem*>", marshal_QTreeWidgetItemList, NULL },
  { "QList<QTreeWidgetItem*>&", marshal_QTreeWidgetItemList, NULL },
  { "QList<QUndoStack*>", marshal_QUndoStackList, NULL },
  { "QList<QUndoStack*>&", marshal_QUndoStackList, NULL },
  { "QList<QUrl>", marshal_QUrlList, NULL },
  { "QList<QUrl>&", marshal_QUrlList, NULL },
  { "QList<QVariant>", marshal_QVariantList, NULL },
  { "QList<QVariant>&", marshal_QVariantList, NULL },
  { "QList<QWidget*>", marshal_QWidgetPtrList, NULL },
  { "QList<QWidget*>&", marshal_QWidgetPtrList, NULL },
  { "qlonglong", marshal_it<long long>, NULL },
  { "qlonglong&", marshal_it<long long>, NULL },
  { "QMap<int,QVariant>", marshal_QMapIntQVariant, NULL },
  { "QMap<int,QVariant>&", marshal_QMapIntQVariant, NULL },
  { "QMap<QString,QString>", marshal_QMapQStringQString, NULL },
  { "QMap<QString,QString>&", marshal_QMapQStringQString, NULL },
  { "QMap<QString,QVariant>", marshal_QMapQStringQVariant, NULL },
  // FIXME: need QHash variant for the above
  { "QMap<QString,QVariant>&", marshal_QMapQStringQVariant, NULL },
  { "QVariantMap", marshal_QMapQStringQVariant, NULL },
  { "QVariantMap&", marshal_QMapQStringQVariant, NULL },
  { "QModelIndexList", marshal_QModelIndexList, NULL },
  { "QModelIndexList&", marshal_QModelIndexList, NULL },
  { "QObjectList", marshal_QObjectList, NULL },
  { "QObjectList&", marshal_QObjectList, NULL },
  { "QPair<int,int>&", marshal_QPairintint, NULL },
  { "Q_PID", marshal_it<Q_PID>, NULL },
  { "qreal*", marshal_doubleR, NULL },
  { "qreal&", marshal_doubleR, NULL },
  { "QRgb*", marshal_QRgb_array, NULL },
  { "QStringList", marshal_QStringList, NULL },
  { "QStringList*", marshal_QStringList, NULL },
  { "QStringList&", marshal_QStringList, NULL },
  { "QString", marshal_QString, NULL },
  { "QString*", marshal_QString, NULL },
  { "QString&", marshal_QString, NULL },
  { "QByteArray", marshal_QByteArray, NULL },
  { "QByteArray*", marshal_QByteArray, NULL },
  { "QByteArray&", marshal_QByteArray, NULL },
  { "quint64", marshal_it<unsigned long long>, NULL },
  { "quint64&", marshal_it<unsigned long long>, NULL },
  { "qulonglong", marshal_it<unsigned long long>, NULL },
  { "qulonglong&", marshal_it<unsigned long long>, NULL },
  { "QVariantList&", marshal_QVariantList, NULL },
  { "QVector<int>", marshal_QVectorint, NULL },
  { "QVector<int>&", marshal_QVectorint, NULL },
  { "QVector<QColor>", marshal_QColorVector, NULL },
  { "QVector<QColor>&", marshal_QColorVector, NULL },
  { "QVector<QLineF>", marshal_QLineFVector, NULL },
  { "QVector<QLineF>&", marshal_QLineFVector, NULL },
  { "QVector<QLine>", marshal_QLineVector, NULL },
  { "QVector<QLine>&", marshal_QLineVector, NULL },
  { "QVector<QPointF>", marshal_QPointFVector, NULL },
  { "QVector<QPointF>&", marshal_QPointFVector, NULL },
  { "QVector<QPoint>", marshal_QPointVector, NULL },
  { "QVector<QPoint>&", marshal_QPointVector, NULL },
  { "QVector<qreal>", marshal_QVectorqreal, NULL },
  { "QVector<qreal>&", marshal_QVectorqreal, NULL },
  { "QVector<QRectF>", marshal_QRectFVector, NULL },
  { "QVector<QRectF>&", marshal_QRectFVector, NULL },
  { "QVector<QRect>", marshal_QRectVector, NULL },
  { "QVector<QRect>&", marshal_QRectVector, NULL },
  { "QVector<QRgb>", marshal_QRgbVector, NULL },
  { "QVector<QRgb>&", marshal_QRgbVector, NULL },
  { "QVector<QTextFormat>", marshal_QTextFormatVector, NULL },
  { "QVector<QTextFormat>&", marshal_QTextFormatVector, NULL },
  { "QVector<QTextLength>", marshal_QTextLengthVector, NULL },
  { "QVector<QTextLength>&", marshal_QTextLengthVector, NULL },
  { "QVector<QVariant>", marshal_QVariantVector, NULL },
  { "QVector<QVariant>&", marshal_QVariantVector, NULL },
  { "QWidgetList", marshal_QWidgetList, NULL },
  { "QWidgetList&", marshal_QWidgetList, NULL },
  { "QwtArray<double>", marshal_QVectorqreal, NULL },
  { "QwtArray<double>&", marshal_QVectorqreal, NULL },
  { "QwtArray<int>", marshal_QVectorint, NULL },
  { "QwtArray<int>&", marshal_QVectorint, NULL },
  { "signed int&", marshal_it<int *>, NULL },
  { "const uchar*", marshal_it<const unsigned char *>, NULL },
  //{ "uchar*", marshal_it<unsigned char *>, NULL },
  { "unsigned long long int", marshal_it<long long>, NULL },
  { "unsigned long long int&", marshal_it<long long>, NULL },
  { "void", marshal_void, NULL },
  { "void**", marshal_voidP_array, NULL },
  { "WId", marshal_it<WId>, NULL },
#if QT_VERSION >= 0x40200
  { "QList<QGraphicsItem*>", marshal_QGraphicsItemList, NULL },
  { "QList<QGraphicsItem*>&", marshal_QGraphicsItemList, NULL },
  { "QList<QStandardItem*>", marshal_QStandardItemList, NULL },
  { "QList<QStandardItem*>&", marshal_QStandardItemList, NULL },
  { "QList<QUndoStack*>", marshal_QUndoStackList, NULL },
  { "QList<QUndoStack*>&", marshal_QUndoStackList, NULL },
#endif
#if QT_VERSION >= 0x40300
  { "QList<QMdiSubWindow*>", marshal_QMdiSubWindowList, NULL },
  { "QList<QSslCertificate>", marshal_QSslCertificateList, NULL },
  { "QList<QSslCertificate>&", marshal_QSslCertificateList, NULL },
  { "QList<QSslCipher>", marshal_QSslCipherList, NULL },
  { "QList<QSslCipher>&", marshal_QSslCipherList, NULL },
  { "QList<QSslError>", marshal_QSslErrorList, NULL },
  { "QList<QSslError>&", marshal_QSslErrorList, NULL },
  { "QXmlStreamEntityDeclarations", marshal_QXmlStreamEntityDeclarations, NULL },
  { "QXmlStreamNamespaceDeclarations", marshal_QXmlStreamNamespaceDeclarations, NULL },
  { "QXmlStreamNotationDeclarations", marshal_QXmlStreamNotationDeclarations, NULL },
#endif
#if QT_VERSION >= 0x040400
  { "QList<QNetworkCookie>", marshal_QNetworkCookieList, NULL },
  { "QList<QNetworkCookie>&", marshal_QNetworkCookieList, NULL },
  { "QList<QPrinterInfo>", marshal_QPrinterInfoList, NULL },
#endif
  { 0, 0, NULL }
};
