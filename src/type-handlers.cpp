/* TypeHandler implementation:
   Marshalling and scoring method arguments
*/

/* Based on code from QtRuby */

/* Type conversion thoughts:
      
   Enums: It does not seem like Smoke provides the string name of enum
   values. We can get all the values for a particular class, but the
   values not grouped by enum type. Qt does not help much either,
   because QMetaEnum only works for enums defined in a class. Many are
   not, and how would we get the class anyway?
   
   Values: Many value types (e.g. QRectF) are often easier to
   manipulate as native R objects (e.g. vectors). The problem is, we
   cannot simply always convert them and mark them as Qt objects, as
   inconsistencies would be introduced by non-const methods, where by
   the object is simultaneously copy-on-write and a reference. A few
   classes, like QTransform, have non-const methods that return
   themselves after a modification. While this does not always copy in
   C++, we can perform the copy. Any classes that do not follow this
   convention are a problem. The simplest path is to require explicit
   coercion for every case.

   Implicit conversions:
     QVariant <-> R value (R programmer does not care about this C++ ism)
     QString <-> character (obvious)
     QByteArray <- raw (other direction is explicit, for performance, features)
     QGenericMatrix <-> R matrix (is a voidp, no other choice really)
     
   Converters that we have:
     QRect(F) -> 2x2 matrix [as.matrix()],
     QPoint(F) -> 2-vector [as.vector()],
     QSize(F) -> 2-vector [as.vector()],
     QTransform -> 3x3 matrix [as.matrix()]
     QColor -> 4x1 matrix, [qcol2rgb(), as.matrix()]
     QByteArray -> raw vector [as.raw()]
     
   On an as-needed basis:
     QDate/QTime: from R date and time objects [as.Date(), as.POSIX..]
     QBitArray: from an R raw vector [as.raw()]
     QTableWidgetSelectionRange: 2x2 matrix [as.matrix()]
     QMatrix4x4: as R matrix [as.matrix()]
     QVector(2D,3D,4D): as R vectors [as.vector()]
     
   References to primitives: out parameters or arrays? QtRuby treats
   them as out parameters, but QFontMetrics::size() is one example
   where int* is an array. Arrays are tricky; not supported.

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

/* TODO: support QDBusReply:
   QDBusReply<QDBusConnectionInterface::RegisterServiceReply>
   QDBusReply<QString>
   QDBusReply<QStringList>
   QDBusReply<bool>
   QDBusReply<unsigned int>
   QDBusReply<void>
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
#include <QDate>
#include <QtGui/qabstractbutton.h>
#include <QtGui/qabstracttextdocumentlayout.h>
#include <QtGui/qaccessible.h>
#include <QtGui/qaction.h>
#include <QtGui/qapplication.h>
#include <QtGui/qdockwidget.h>
#include <QtGui/qevent.h>
#include <QtGui/qfontdatabase.h>
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

#ifdef QT_NETWORK_LIB
#include <QtNetwork/qhostaddress.h>
#include <QtNetwork/qnetworkinterface.h>
#include <QtNetwork/qurlinfo.h>
#endif

#if QT_VERSION >= 0x40100
#ifdef QT_NETWORK_LIB
#include <QtNetwork/qnetworkproxy.h>
#endif
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
#include <QtGui/qwizard.h>
#ifdef QT_NETWORK_LIB
#include <QtNetwork/qsslcertificate.h>
#include <QtNetwork/qsslcipher.h>
#include <QtNetwork/qsslerror.h>
#endif
#ifdef QT_XML_LIB
#include <QtXml/qxmlstream.h>
#endif
#endif

#if QT_VERSION >= 0x040400
#include <QtGui/qprinterinfo.h>
#ifdef QT_NETWORK_LIB
#include <QtNetwork/qnetworkcookie.h>
#include <QtNetwork/qnetworkrequest.h>
#endif
#endif

#if QT_VERSION >= 0x040600
#include <QAbstractAnimation>
#include <QAbstractState>
#include <QGenericMatrix>
#ifdef QT_OPENGL_LIB
#include <QGLShader>
#endif
#endif

#include <smoke.h>
#include "type-handlers.hpp"
#include "DynamicBinding.hpp"
#include "Class.hpp"

#include <Rdefines.h> // syntax more convenient for macros

#undef isNull // R causing trouble again

/* instance marshaling */

template <>
void marshal_from_sexp<SmokeClassWrapper>(MethodCall *m)
{
  SEXP v = m->sexp();
  SmokeType type = m->type();
  SmokeObject *o = NULL, *coerced = NULL;
  
  if (v != R_NilValue) {
    o = SmokeObject::fromSexp(v);
    if (!o->instanceOf(type.className())) {
      // attempt implicit conversion
      coerced = o->convertImplicitly(type);
      if (coerced)
        o = coerced;
      else error("Failed to coerce an instance of type '%s' to '%s'",
                 o->className(), type.className());
    }
  }
  
  if (o && m->returning() && !type.fitsStack()) {
    o = o->clone(); // Smoke takes ownership of virtual returns on the stack
  }

  void *ptr = o ? o->castPtr(type.className()) : NULL;
  setItemValue(m, ptr);

  m->marshal();

  if (coerced)
    delete coerced;
  
  return;
}

template <>
void marshal_to_sexp<SmokeClassWrapper>(MethodCall *m)
{
  void *p = itemValue<void *>(m);
  SEXP sexp = ptr_to_sexp(p, m->type());
  /* NOTE: This can lead to memory leaks for objects created via
     factory methods. But the alternative is the possibility for
     seg-faults, because if an object is not a Smoke instance, we will
     not know when it is deleted. Granted, the R user can still call a
     dead object and crash R, but at least the memory management code
     will be well behaved.
  */
  if (sexp != R_NilValue &&
      !(m->returning() && m->method()->qualifiers() & Method::Constructor))
    SmokeObject::fromSexp(sexp)->setAllocated(false);
  m->setSexp(sexp);
}

void marshal_basetype(MethodCall *m)
{
  switch(m->type().elem()) {

  case Smoke::t_bool:
    marshal<bool>(m);
    break;

  case Smoke::t_char:
    marshal<signed char>(m);
    break;
		
  case Smoke::t_uchar:
    marshal<unsigned char>(m);
    break;
 
  case Smoke::t_short:
    marshal<short>(m);
    break;
      
  case Smoke::t_ushort:
    marshal<unsigned short>(m);
    break;

  case Smoke::t_int:
    marshal<int>(m);
    break;
		
  case Smoke::t_uint:
    marshal<unsigned int>(m);
    break;
 
  case Smoke::t_long:
    marshal<long>(m);
    break;

  case Smoke::t_ulong:
    marshal<unsigned long>(m);
    break;
 
  case Smoke::t_float:
    marshal<float>(m);
    break;

  case Smoke::t_double:
    marshal<double>(m);
    break;

  case Smoke::t_enum:
    marshal<SmokeEnumWrapper>(m);
    break;
     
  case Smoke::t_class:
    {
      if (!qstrcmp(m->type().className(), "QVariant"))
        marshal<QVariant>(m); /* special-case QVariant */
      else marshal<SmokeClassWrapper>(m);
    }
    break;

  default:
    m->unsupported();
    break;	
  }

}

void marshal_void(MethodCall * /*m*/) {}

/* Unknown types are those treated as void pointers by Smoke while
   lacking an explicit handler. In general, we cannot handle stack
   types (need to allocate/free them sometimes), or reference types
   (need to copy the const ones, for example). However, we do try to
   handle the pointer types by just wrapping them in an externalptr.
*/
void marshal_unknown(MethodCall *m) {
  if (m->type().isPtr())
    marshal<SmokePtrWrapper>(m);
  else m->unsupported();
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
        const char *smokeClass = type.className();
        if (o->className() == smokeClass)
          score = 3;
        else if (o->instanceOf(smokeClass))
          score = 2;
        else {
          Method *m = Class::fromSmokeType(type)->findImplicitConverter(o);
          if (m) {
            score = 1;
            delete m;
          }
        }
      }
    }
    break;
  case NILSXP:
    if (type.isPtr())
      score = 1;
    break;
  default:
    break;
  }
  return score;
}

int scoreArg_unknown(SEXP /*arg*/, const SmokeType &type) {
  error("unable to score argument of type '%s'", type.name());
  return 0;
}

template<> int scoreArg<QString>(SEXP arg, const SmokeType &type) {
  if (TYPEOF(arg) == STRSXP)
    return type.isPtr() ? 1 : 3;
  else return 0;
}

template<> int scoreArg<const char*>(SEXP arg, const SmokeType &type) {
  if (TYPEOF(arg) == STRSXP)
    return 2;
  else return 0;
}

template <> int scoreArg<QStringList>(SEXP arg, const SmokeType &/*type*/) {
  if (TYPEOF(arg) == STRSXP) {
    if (length(arg) > 1)
      return 3;
    return 2;
  }
  else return 0;
}

template<> int scoreArg<QByteArray>(SEXP arg, const SmokeType &type) {
  if (TYPEOF(arg) == STRSXP)
    return type.isPtr() ? 1 : 2;
  else if (TYPEOF(arg) == RAWSXP)
    return type.isPtr() ? 2 : 3;
  else return scoreArg_basetype(arg, type);
}

template<> int scoreArg<QVariant>(SEXP arg, const SmokeType &type) {
  Q_UNUSED(arg);
  Q_UNUSED(type);
  return 2;
}

/* Converter definitions */

DEF_PAIR_CONVERTERS(double, QColor, value, class)
DEF_PAIR_CONVERTERS(double, QVariant, value, value)
DEF_PAIR_CONVERTERS(QString, QString, value, value)
DEF_PAIR_CONVERTERS(QByteArray, QByteArray, class, class)
DEF_PAIR_CONVERTERS(double, QPointF, value, class)
DEF_PAIR_CONVERTERS(double, double, value, value)
DEF_PAIR_CONVERTERS(int, int, value, value)

DEF_COLLECTION_CONVERTERS(QList, QAbstractButton*, ptr)
DEF_COLLECTION_CONVERTERS(QList, QActionGroup*, ptr)
DEF_COLLECTION_CONVERTERS(QList, QAction*, ptr)
DEF_COLLECTION_CONVERTERS(QList, QDockWidget*, ptr)
DEF_COLLECTION_CONVERTERS(QList, QListWidgetItem*, ptr)
DEF_COLLECTION_CONVERTERS(QList, QObject*, ptr)
DEF_COLLECTION_CONVERTERS(QList, QTableWidget*, ptr)
DEF_COLLECTION_CONVERTERS(QList, QTableWidgetItem*, ptr)
DEF_COLLECTION_CONVERTERS(QList, QTextFrame*, ptr)
DEF_COLLECTION_CONVERTERS(QList, QTreeWidgetItem*, ptr)
DEF_COLLECTION_CONVERTERS(QList, QTreeWidget*, ptr)
DEF_COLLECTION_CONVERTERS(QList, QWidget*, ptr)

DEF_COLLECTION_CONVERTERS(QList, QByteArray, class)
DEF_COLLECTION_CONVERTERS(QVector, QColor, class)
DEF_COLLECTION_CONVERTERS(QList, QColor, class)
DEF_COLLECTION_CONVERTERS(QList, QFileInfo, class)
DEF_COLLECTION_CONVERTERS(QList, QImageTextKeyLang, class)
DEF_COLLECTION_CONVERTERS(QList, QKeySequence, class)
DEF_COLLECTION_CONVERTERS(QVector, QLineF, class)
DEF_COLLECTION_CONVERTERS(QVector, QLine, class)
DEF_COLLECTION_CONVERTERS(QList, QModelIndex, class)
DEF_COLLECTION_CONVERTERS(QList, QPixmap, class)
DEF_COLLECTION_CONVERTERS(QVector, QPointF, class)
DEF_COLLECTION_CONVERTERS(QVector, QPoint, class)
DEF_COLLECTION_CONVERTERS(QList, QPolygonF, class)
DEF_COLLECTION_CONVERTERS(QList, QRectF, class)
DEF_COLLECTION_CONVERTERS(QVector, QRectF, class)
DEF_COLLECTION_CONVERTERS(QVector, QRect, class)
DEF_COLLECTION_CONVERTERS(QVector, QRgb, class)
DEF_COLLECTION_CONVERTERS(QList, QTableWidgetSelectionRange, class)
DEF_COLLECTION_CONVERTERS(QList, QTextBlock, class)
DEF_COLLECTION_CONVERTERS(QVector, QTextFormat, class)
DEF_COLLECTION_CONVERTERS(QVector, QTextLength, class)
DEF_COLLECTION_CONVERTERS(QList, QSizeF, class)
DEF_COLLECTION_CONVERTERS(QList, QUrl, class)
DEF_COLLECTION_CONVERTERS(QList, QDate, class)
DEF_COLLECTION_CONVERTERS(QList, QTextCharFormat, class)
DEF_COLLECTION_CONVERTERS(QList, QTextEdit::ExtraSelection, class)
DEF_COLLECTION_CONVERTERS(QList, QTextLayout::FormatRange, class)
DEF_COLLECTION_CONVERTERS(QList, QInputMethodEvent::Attribute, class)
DEF_COLLECTION_CONVERTERS(QVector, QTextLayout::FormatRange, class)
DEF_COLLECTION_CONVERTERS(QList, QTextOption::Tab, class)
DEF_COLLECTION_CONVERTERS(QList, QAbstractTextDocumentLayout::Selection, class)
DEF_COLLECTION_CONVERTERS(QVector, QAbstractTextDocumentLayout::Selection,
                          class)

DEF_COLLECTION_CONVERTERS_PAIR(QList, double, QColor, value)
DEF_COLLECTION_CONVERTERS_PAIR(QList, QString, QString, value)
DEF_COLLECTION_CONVERTERS_PAIR(QVector, double, QColor, value)
DEF_COLLECTION_CONVERTERS_PAIR(QVector, double, QVariant, value)
DEF_COLLECTION_CONVERTERS_PAIR(QList, QByteArray, QByteArray, value)
DEF_COLLECTION_CONVERTERS_PAIR(QList, double, QPointF, value)
DEF_COLLECTION_CONVERTERS_PAIR(QList, double, double, value)
DEF_COLLECTION_CONVERTERS_PAIR(QList, int, int, value)

DEF_PRIM_COLLECTION_CONVERTERS(QList, int, INTEGER)
DEF_PRIM_COLLECTION_CONVERTERS(QList, unsigned int, NUMERIC)
DEF_PRIM_COLLECTION_CONVERTERS(QList, double, NUMERIC)
DEF_COLLECTION_CONVERTERS(QList, QVariant, value)
DEF_COLLECTION_CONVERTERS(QVector, QVariant, value)
DEF_PRIM_COLLECTION_CONVERTERS(QVector, int, INTEGER)
DEF_PRIM_COLLECTION_CONVERTERS(QVector, unsigned int, NUMERIC)
DEF_PRIM_COLLECTION_CONVERTERS(QVector, double, NUMERIC)

DEF_COLLECTION_CONVERTERS(QList, QWizard::WizardButton, enum)
DEF_COLLECTION_CONVERTERS(QList, QFontDatabase::WritingSystem, enum)
DEF_COLLECTION_CONVERTERS(QList, QLocale::Country, enum)
DEF_COLLECTION_CONVERTERS(QList, QPrinter::PageSize, enum)
DEF_COLLECTION_CONVERTERS(QSet, QAccessible::Method, enum)

DEF_MAP_CONVERTERS(QMap, int, QVariant)
DEF_MAP_CONVERTERS(QMap, QDate, QTextCharFormat)
DEF_MAP_CONVERTERS(QHash, int, QByteArray)

#ifdef QT_NETWORK_LIB
DEF_COLLECTION_CONVERTERS(QList, QHostAddress, class)
DEF_COLLECTION_CONVERTERS(QList, QNetworkAddressEntry, class)
DEF_COLLECTION_CONVERTERS(QList, QNetworkInterface, class)
DEF_PAIR_CONVERTERS(QHostAddress, int, class, value)
#endif

#if QT_VERSION >= 0x40100
#ifdef QT_NETWORK_LIB
DEF_COLLECTION_CONVERTERS(QList, QNetworkProxy, class)
#endif
#endif

#if QT_VERSION >= 0x40200
DEF_COLLECTION_CONVERTERS(QList, QGraphicsItem*, ptr)
DEF_COLLECTION_CONVERTERS(QList, QGraphicsView*, ptr)
DEF_COLLECTION_CONVERTERS(QList, QStandardItem*, ptr)
DEF_COLLECTION_CONVERTERS(QList, QUndoStack*, ptr)
#endif

#if QT_VERSION >= 0x40300
DEF_COLLECTION_CONVERTERS(QList, QMdiSubWindow*, ptr)
#ifdef QT_NETWORK_LIB
DEF_COLLECTION_CONVERTERS(QList, QSslCertificate, class)
DEF_COLLECTION_CONVERTERS(QList, QSslCipher, class)
DEF_COLLECTION_CONVERTERS(QList, QSslError, class)
#endif
#if defined(QT_XML_LIB) || QT_VERSION >= 0x40400
DEF_COLLECTION_CONVERTERS(QVector, QXmlStreamEntityDeclaration, class)
DEF_COLLECTION_CONVERTERS(QVector, QXmlStreamNamespaceDeclaration, class)
DEF_COLLECTION_CONVERTERS(QVector, QXmlStreamNotationDeclaration, class)
#endif
#endif

#if QT_VERSION >= 0x40400
DEF_COLLECTION_CONVERTERS(QList, QGraphicsWidget*, ptr)
#ifdef QT_NETWORK_LIB
DEF_COLLECTION_CONVERTERS(QList, QNetworkCookie, class)
DEF_COLLECTION_CONVERTERS(QList, QNetworkRequest::Attribute, enum)
DEF_MAP_CONVERTERS(QHash, QNetworkRequest::Attribute, QVariant)
#endif
DEF_COLLECTION_CONVERTERS(QList, QPrinterInfo, class)
#endif

#if QT_VERSION >= 0x40600
DEF_COLLECTION_CONVERTERS(QList, QAbstractAnimation*, ptr)
DEF_COLLECTION_CONVERTERS(QList, QAbstractState*, ptr)
DEF_COLLECTION_CONVERTERS(QSet, QAbstractState*, ptr)
DEF_COLLECTION_CONVERTERS(QList, QGraphicsTransform*, ptr)
DEF_MATRIX_CONVERTERS(2, 2)
DEF_MATRIX_CONVERTERS(2, 3)
DEF_MATRIX_CONVERTERS(2, 4)
DEF_MATRIX_CONVERTERS(3, 2)
DEF_MATRIX_CONVERTERS(3, 3)
DEF_MATRIX_CONVERTERS(3, 4)
DEF_MATRIX_CONVERTERS(4, 2)
DEF_MATRIX_CONVERTERS(4, 3)
#ifdef QT_OPENGL_LIB
DEF_COLLECTION_CONVERTERS(QList, QGLShader*, ptr)
#endif
#endif

Q_DECL_EXPORT TypeHandler Qt_handlers[] = {
  /* Handle primitive pointers/references */
  TYPE_HANDLER_ENTRY_PRIM(bool),
  /* 'char' is a little special, as 'const (unsigned) char*' is a string */
  TYPE_HANDLER_ENTRY_FULL(char&, char),
  TYPE_HANDLER_ENTRY_FULL(signed char&, char),
  TYPE_HANDLER_ENTRY_FULL(unsigned char&, char),
  TYPE_HANDLER_ENTRY_FULL(char*&, char),
  TYPE_HANDLER_ENTRY_FULL(char*, char),
  TYPE_HANDLER_ENTRY_FULL(unsigned char*, char),
  TYPE_HANDLER_ENTRY_FULL(const char*, const char*),
  TYPE_HANDLER_ENTRY_FULL(const unsigned char*, const char*),
  TYPE_HANDLER_ENTRY_INT(int),
  TYPE_HANDLER_ENTRY_INT(long),
  TYPE_HANDLER_ENTRY_INT(short),
  TYPE_HANDLER_ENTRY_PRIM(double),
  TYPE_HANDLER_ENTRY_PRIM(float),
  TYPE_HANDLER_ENTRY_CLASS(QString),
  TYPE_HANDLER_ENTRY_FULL(QString*, QString),
  TYPE_HANDLER_ENTRY_FULL(QString&, QString),
  TYPE_HANDLER_ENTRY_CLASS(QByteArray),
  TYPE_HANDLER_ENTRY_FULL(QByteArray*, QByteArray),
  /* Handle various collection classes */
  TYPE_HANDLER_ENTRY_CLASS(QList<int>),
  TYPE_HANDLER_ENTRY_CLASS(QList<unsigned int>),
  TYPE_HANDLER_ENTRY_CLASS(QVector<int>),
  TYPE_HANDLER_ENTRY_CLASS(QList<double>),
  TYPE_HANDLER_ENTRY_CLASS(QStringList),
  TYPE_HANDLER_ENTRY_CLASS(QSet<QAccessible::Method>),
  TYPE_HANDLER_ENTRY_CLASS(QList<QWizard::WizardButton>),
  TYPE_HANDLER_ENTRY_CLASS(QList<QFontDatabase::WritingSystem>),
  TYPE_HANDLER_ENTRY_CLASS(QList<QLocale::Country>),
  TYPE_HANDLER_ENTRY_CLASS(QList<QPrinter::PageSize>),
  TYPE_HANDLER_ENTRY_CLASS(QList<QAbstractButton*>),
  TYPE_HANDLER_ENTRY_CLASS(QList<QAction*>),
  TYPE_HANDLER_ENTRY_CLASS(QList<QDockWidget*>),
  TYPE_HANDLER_ENTRY_CLASS(QList<QListWidgetItem*>),
  TYPE_HANDLER_ENTRY_CLASS(QList<QObject*>),
  TYPE_HANDLER_ENTRY_CLASS(QList<QTableWidgetItem*>),
  TYPE_HANDLER_ENTRY_CLASS(QList<QTextFrame*>),
  TYPE_HANDLER_ENTRY_CLASS(QList<QWidget*>),
  TYPE_HANDLER_ENTRY_CLASS(QList<QTreeWidgetItem*>),
  TYPE_HANDLER_ENTRY_CLASS(QList<QUndoStack*>),
  TYPE_HANDLER_ENTRY_CLASS(QList<QByteArray>),
  TYPE_HANDLER_ENTRY_CLASS(QList<QFileInfo>),
  TYPE_HANDLER_ENTRY_CLASS(QList<QFileInfo>),
  TYPE_HANDLER_ENTRY_CLASS(QList<QImageTextKeyLang>),
  TYPE_HANDLER_ENTRY_CLASS(QList<QKeySequence>),
  TYPE_HANDLER_ENTRY_CLASS(QList<QModelIndex>),
  TYPE_HANDLER_ENTRY_CLASS(QList<QPolygonF>),
  TYPE_HANDLER_ENTRY_CLASS(QList<QRectF>),
  TYPE_HANDLER_ENTRY_CLASS(QList<QSizeF>),
  TYPE_HANDLER_ENTRY_CLASS(QList<QTableWidgetSelectionRange>),
  TYPE_HANDLER_ENTRY_CLASS(QList<QTextBlock>),
  TYPE_HANDLER_ENTRY_CLASS(QList<QUrl>),
  TYPE_HANDLER_ENTRY_CLASS(QList<QVariant>),
  TYPE_HANDLER_ENTRY_CLASS(QList<QColor>),
  TYPE_HANDLER_ENTRY_CLASS(QList<QTextEdit::ExtraSelection>),
  TYPE_HANDLER_ENTRY_CLASS(QList<QTextLayout::FormatRange>),
  TYPE_HANDLER_ENTRY_CLASS(QVector<QTextLayout::FormatRange>),
  TYPE_HANDLER_ENTRY_CLASS(QList<QInputMethodEvent::Attribute>),
  TYPE_HANDLER_ENTRY_CLASS(QList<QAbstractTextDocumentLayout::Selection>),
  TYPE_HANDLER_ENTRY_CLASS(QVector<QAbstractTextDocumentLayout::Selection>),
  TYPE_HANDLER_ENTRY_CLASS(QList<QTextOption::Tab>),
  TYPE_HANDLER_ENTRY_CLASS(QVector<QLine>),
  TYPE_HANDLER_ENTRY_CLASS(QVector<QLineF>),
  TYPE_HANDLER_ENTRY_CLASS(QVector<QPointF>),
  TYPE_HANDLER_ENTRY_CLASS(QVector<QPoint>),
  TYPE_HANDLER_ENTRY_CLASS(QVector<QRectF>),
  TYPE_HANDLER_ENTRY_CLASS(QVector<QRect>),
  TYPE_HANDLER_ENTRY_CLASS(QVector<QTextFormat>),
  TYPE_HANDLER_ENTRY_CLASS(QVector<QTextLength>),
  TYPE_HANDLER_ENTRY_CLASS(QVector<QVariant>),
  TYPE_HANDLER_ENTRY_CLASS(QVector<double>),
  TYPE_HANDLER_ENTRY_CLASS2(QList<QPair<double,QColor> >),
  TYPE_HANDLER_ENTRY_CLASS2(QList<QPair<QString,QString> >),
  TYPE_HANDLER_ENTRY_CLASS2(QList<QPair<QByteArray,QByteArray> >),
  TYPE_HANDLER_ENTRY_CLASS2(QList<QPair<double,QPointF> >),
  TYPE_HANDLER_ENTRY_CLASS2(QList<QPair<double,double> >),
  TYPE_HANDLER_ENTRY_CLASS2(QList<QPair<int,int> >),
  TYPE_HANDLER_ENTRY_CLASS2(QVector<QPair<double, QColor> >),
  TYPE_HANDLER_ENTRY_CLASS2(QVector<QPair<double, QVariant> >),
  TYPE_HANDLER_ENTRY_CLASS2(QMap<int,QVariant>),
  TYPE_HANDLER_ENTRY_CLASS2(QHash<int,QByteArray>),
  TYPE_HANDLER_ENTRY_CLASS2(QMap<QDate,QTextCharFormat>),
  TYPE_HANDLER_ENTRY_CLASS2(QPair<int,int>),
#ifdef QT_OPENGL_LIB
#if QT_VERSION >= 0x40600
  TYPE_HANDLER_ENTRY_CLASS(QList<QGLShader*>),
#endif    
#endif    
#ifdef QT_NETWORK_LIB
  TYPE_HANDLER_ENTRY_CLASS(QList<QHostAddress>),
  TYPE_HANDLER_ENTRY_CLASS(QList<QNetworkAddressEntry>),
  TYPE_HANDLER_ENTRY_CLASS(QList<QNetworkInterface>),
  TYPE_HANDLER_ENTRY_CLASS(QList<QNetworkCookie>),
  TYPE_HANDLER_ENTRY_CLASS(QList<QNetworkProxy>),
  TYPE_HANDLER_ENTRY_CLASS2(QPair<QHostAddress,int>),
  TYPE_HANDLER_ENTRY_CLASS2(QHash<QNetworkRequest::Attribute,QVariant>),
#endif
#if defined(QT_XML_LIB) || QT_VERSION >= 0x40400
  TYPE_HANDLER_ENTRY_CLASS(QVector<QXmlStreamEntityDeclaration>),
  TYPE_HANDLER_ENTRY_CLASS(QVector<QXmlStreamNamespaceDeclaration>),
  TYPE_HANDLER_ENTRY_CLASS(QVector<QXmlStreamNotationDeclaration>),
#endif
#if QT_VERSION >= 0x40200
  TYPE_HANDLER_ENTRY_CLASS(QList<QGraphicsItem*>),
  TYPE_HANDLER_ENTRY_CLASS(QList<QGraphicsView*>),
  TYPE_HANDLER_ENTRY_CLASS(QList<QGraphicsWidget*>),
  TYPE_HANDLER_ENTRY_CLASS(QList<QStandardItem*>),
#endif
#if QT_VERSION >= 0x40300
  TYPE_HANDLER_ENTRY_CLASS(QList<QMdiSubWindow*>),
#ifdef QT_NETWORK_LIB
  TYPE_HANDLER_ENTRY_CLASS(QList<QSslCertificate>),
  TYPE_HANDLER_ENTRY_CLASS(QList<QSslCipher>),
  TYPE_HANDLER_ENTRY_CLASS(QList<QSslError>),
#endif
#endif
#if QT_VERSION >= 0x040400
  TYPE_HANDLER_ENTRY_CLASS(QList<QPrinterInfo>),
#ifdef QT_NETWORK_LIB
  TYPE_HANDLER_ENTRY_CLASS(QList<QNetworkCookie>),
#endif
#endif
#if QT_VERSION >= 0x40600
  TYPE_HANDLER_ENTRY_CLASS(QList<QGraphicsTransform*>),
  TYPE_HANDLER_ENTRY_CLASS(QList<QAbstractAnimation*>),
  TYPE_HANDLER_ENTRY_CLASS(QList<QAbstractState*>),
  TYPE_HANDLER_ENTRY_CLASS(QSet<QAbstractState*>),
  /* matrices */
  TYPE_HANDLER_ENTRY_CLASS3(QGenericMatrix<2,2,double>),
  TYPE_HANDLER_ENTRY_CLASS3(QGenericMatrix<2,3,double>),
  TYPE_HANDLER_ENTRY_CLASS3(QGenericMatrix<2,4,double>),
  TYPE_HANDLER_ENTRY_CLASS3(QGenericMatrix<3,2,double>),
  TYPE_HANDLER_ENTRY_CLASS3(QGenericMatrix<3,2,double>),
  TYPE_HANDLER_ENTRY_CLASS3(QGenericMatrix<3,3,double>),
  TYPE_HANDLER_ENTRY_CLASS3(QGenericMatrix<3,4,double>),
  TYPE_HANDLER_ENTRY_CLASS3(QGenericMatrix<4,2,double>),
  TYPE_HANDLER_ENTRY_CLASS3(QGenericMatrix<4,3,double>),
#endif
  /*************** Special cases ***************/
  /* long long */
  TYPE_HANDLER_ENTRY(long long),
  TYPE_HANDLER_ENTRY_FULL(long long*, long long),
  TYPE_HANDLER_ENTRY(unsigned long long),
  /* opaque pointers */
  TYPE_HANDLER_ENTRY(void*),
  TYPE_HANDLER_ENTRY_FULL(const void*, void*),
  /* primitive typedefs */
  TYPE_HANDLER_ENTRY_FULL(size_t, unsigned int),
  TYPE_HANDLER_ENTRY_FULL(GLenum, int),
  TYPE_HANDLER_ENTRY_FULL(GLfloat, float),
  TYPE_HANDLER_ENTRY_FULL(GLint, int),
  TYPE_HANDLER_ENTRY_FULL(GLuint, unsigned int),
  TYPE_HANDLER_ENTRY_FULL(GLbitfield, unsigned int),
  { 0, 0, NULL }
};

void init_type_handlers() {
  // FIXME: do we want to integrate this with RQtModule?
  MethodCall::registerTypeHandlers(Qt_handlers);
}
