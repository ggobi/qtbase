#include <smoke/qt_smoke.h>

/* For QVariant conversion */
#include <QCursor>
#include <QDate>
#include <QSize>
#include <QSizeF>
#include <QTime>
#include <QPolygon>
#include <QColor>
#include <QRectF>
#include <QRect>
#include <QLine>
#include <QLineF>
#include <QTextLength>
#include <QStringList>
#include <QIcon>
#include <QPen>
#include <QTextFormat>
#include <QPoint>
#include <QPointF>
#include <QUrl>
#include <QRegExp>
#include <QDateTime>
#include <QPalette>
#include <QFont>
#include <QBrush>
#include <QRegion>
#include <QBitArray>
#include <QImage>
#include <QKeySequence>
#include <QSizePolicy>
#include <QPixmap>
#include <QLocale>
#include <QBitmap>
#include <QMatrix>
#if QT_VERSION >= 0x40300
#include <QTransform>
#endif
#if QT_VERSION >= 0x40600
#include <QMatrix4x4>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QQuaternion>
#endif
/* End QVariant includes */

#include "SmokeObject.hpp"
#include "convert.hpp"

#undef isNull

#define QVARIANT_FROM_SEXP(sexp, type)                                  \
  qVariantFromValue(class_from_sexp<type>(sexp, SmokeType(qt_Smoke, #type)))

#define QVARIANT_TO_SEXP(variant, type)                                 \
  class_to_sexp(variant.value<type>(), SmokeType(qt_Smoke, #type))

DEF_STRING_MAP_CONVERTERS(QMap, QVariant)
#if QT_VERSION >= 0x40500
DEF_STRING_MAP_CONVERTERS(QHash, QVariant)
#endif

SEXP to_sexp(QVariant variant) {
  SEXP ans = NULL;
  switch(variant.type()) {
  case QMetaType::Void:
    ans = R_NilValue;
    break;
  case QMetaType::Bool:
    ans = ScalarLogical(variant.value<bool>());
    break;
  case QMetaType::Int:
  case QMetaType::UInt:
  case QMetaType::Long:
  case QMetaType::Short:
  case QMetaType::UShort:
    ans = ScalarInteger(variant.value<int>());
    break;
  case QMetaType::Double:
  case QMetaType::LongLong:
  case QMetaType::ULong:
  case QMetaType::ULongLong:
  case QMetaType::Float:
    ans = ScalarReal(variant.value<double>());
    break;
  case QMetaType::QChar:
  case QMetaType::Char:
  case QMetaType::UChar:
  case QMetaType::QString:
  case QMetaType::QByteArray:
    ans = qstring2sexp(variant.value<QString>());
    break;
  case QMetaType::VoidStar:
    ans = wrapPointer(variant.value<void *>());
    break;
  case QMetaType::QObjectStar:
    ans = ptr_to_sexp(variant.value<QObject *>(),
                      SmokeType(qt_Smoke, "QObject"));
    break;
  case QMetaType::QWidgetStar:
    ans = ptr_to_sexp(variant.value<QWidget *>(),
                      SmokeType(qt_Smoke, "QWidget"));
    break;
  case QMetaType::QCursor:
    ans = QVARIANT_TO_SEXP(variant, QCursor);
    break;
  case QMetaType::QDate:
    ans = QVARIANT_TO_SEXP(variant, QDate);
    break;
  case QMetaType::QSize:
    ans = QVARIANT_TO_SEXP(variant, QSize);
  case QMetaType::QSizeF:
    ans = QVARIANT_TO_SEXP(variant, QSizeF);
    break;
  case QMetaType::QTime:
    ans = QVARIANT_TO_SEXP(variant, QTime);
    break;
  case QMetaType::QVariantList:
    ans = to_sexp(variant.value<QVariantList>(),
                  SmokeType(qt_Smoke, "QList<QVariant>"));
    break;
  case QMetaType::QPolygon:
    ans = QVARIANT_TO_SEXP(variant, QPolygon);
    break;
  case QMetaType::QColor:
    ans = QVARIANT_TO_SEXP(variant, QColor);
    break;
  case QMetaType::QRectF:
    ans = QVARIANT_TO_SEXP(variant, QRectF);
    break;
  case QMetaType::QRect:
    ans = QVARIANT_TO_SEXP(variant, QRect);
    break;
  case QMetaType::QLine:
    ans = QVARIANT_TO_SEXP(variant, QLine);
    break;
  case QMetaType::QTextLength:
    ans = QVARIANT_TO_SEXP(variant, QTextLength);
    break;
  case QMetaType::QStringList:
    ans = to_sexp(variant.value<QStringList>(),
                  SmokeType(qt_Smoke, "QStringList"));
    break;
  case QMetaType::QVariantMap:
    ans = to_sexp(variant.value<QVariantMap>(),
                  SmokeType(qt_Smoke, "QMap<QString,QVariant>"));
    break;
  case QMetaType::QVariantHash:
    ans = to_sexp(variant.value<QVariantHash>(),
                  SmokeType(qt_Smoke, "QHash<QString,QVariant>"));
    break;
  case QMetaType::QIcon:
    ans = QVARIANT_TO_SEXP(variant, QIcon);
    break;
  case QMetaType::QPen:
    ans = QVARIANT_TO_SEXP(variant, QPen);
    break;
  case QMetaType::QLineF:
    ans = QVARIANT_TO_SEXP(variant, QLineF);
    break;
  case QMetaType::QTextFormat:
    ans = QVARIANT_TO_SEXP(variant, QTextFormat);
    break;
  case QMetaType::QPoint:
    ans = QVARIANT_TO_SEXP(variant, QPoint);
    break;
  case QMetaType::QPointF:
    ans = QVARIANT_TO_SEXP(variant, QPointF);
    break;
  case QMetaType::QUrl:
    ans = QVARIANT_TO_SEXP(variant, QUrl);
    break;
  case QMetaType::QRegExp:
    ans = QVARIANT_TO_SEXP(variant, QRegExp);
    break;
  case QMetaType::QDateTime:
    ans = QVARIANT_TO_SEXP(variant, QDateTime);
    break;
  case QMetaType::QPalette:
    ans = QVARIANT_TO_SEXP(variant, QPalette);
    break;
  case QMetaType::QFont:
    ans = QVARIANT_TO_SEXP(variant, QFont);
    break;
  case QMetaType::QBrush:
    ans = QVARIANT_TO_SEXP(variant, QBrush);
    break;
  case QMetaType::QRegion:
    ans = QVARIANT_TO_SEXP(variant, QRegion);
    break;
  case QMetaType::QBitArray:
    ans = QVARIANT_TO_SEXP(variant, QBitArray);
    break;
  case QMetaType::QImage:
    ans = QVARIANT_TO_SEXP(variant, QImage);
    break;
  case QMetaType::QKeySequence:
    ans = QVARIANT_TO_SEXP(variant, QKeySequence);
    break;
  case QMetaType::QSizePolicy:
    ans = QVARIANT_TO_SEXP(variant, QSizePolicy);
    break;
  case QMetaType::QPixmap:
    ans = QVARIANT_TO_SEXP(variant, QPixmap);
    break;
  case QMetaType::QLocale:
    ans = QVARIANT_TO_SEXP(variant, QLocale);
    break;
  case QMetaType::QBitmap:
    ans = QVARIANT_TO_SEXP(variant, QBitmap);
    break;
  case QMetaType::QMatrix: /* obsolete */
    ans = QVARIANT_TO_SEXP(variant, QMatrix);
    break;
#if QT_VERSION >= 0x40300
  case QMetaType::QTransform:
    ans = QVARIANT_TO_SEXP(variant, QTransform);
    break;
#endif
#if QT_VERSION >= 0x40600
  case QMetaType::QMatrix4x4:
    ans = QVARIANT_TO_SEXP(variant, QMatrix4x4);
    break;
  case QMetaType::QVector2D:
    ans = QVARIANT_TO_SEXP(variant, QVector2D);
    break;
  case QMetaType::QVector3D:
    ans = QVARIANT_TO_SEXP(variant, QVector3D);
    break;
  case QMetaType::QVector4D:
    ans = QVARIANT_TO_SEXP(variant, QVector4D);
    break;
  case QMetaType::QQuaternion:
    ans = QVARIANT_TO_SEXP(variant, QQuaternion);
    break;
#endif
  case QMetaType::User:
    break;
  default:
    error("Converting from QVariant: unhandled Qt type");
  }
  if (!ans)
    error("Converting from QVariant: Qt type not yet implemented");
  return ans;
}

/* create a QVariant directly from an R object */
template<> QVariant from_sexp<QVariant>(SEXP rvalue) {
  QVariant variant;
  if (length(rvalue) > 1) {
    SEXP rlist = coerceVector(rvalue, VECSXP);
    if (getAttrib(rvalue, R_NamesSymbol) != R_NilValue)
      variant = asQVariantOfType(rlist, QMetaType::QVariantMap);
    else variant = asQVariantOfType(rlist, QMetaType::QVariantList);
    return variant;
  }
  switch(TYPEOF(rvalue)) {
  case LGLSXP:
    // Rprintf("Logical\n");
    variant = QVariant(asLogical(rvalue));
    break;
  case REALSXP:
    // Rprintf("Real\n");
    variant = QVariant(asReal(rvalue));
    break;
  case INTSXP:
    // Rprintf("Integer\n");
    variant = QVariant(asInteger(rvalue));
    break;
  case STRSXP:
    // Rprintf("String\n");
    variant = QVariant(sexp2qstring(rvalue));
    break;
  case EXTPTRSXP:
    // Rprintf("External pointer\n");
    variant = qVariantFromValue(unwrapPointer(rvalue, void));
    break;
  case ENVSXP: {
    SmokeObject *so = SmokeObject::fromSexp(rvalue);
    if (so->instanceOf("QWidget"))
      variant =
        qVariantFromValue(reinterpret_cast<QWidget *>(so->castPtr("QWidget")));
    else if (so->instanceOf("QObject"))
      variant =
        qVariantFromValue(reinterpret_cast<QObject *>(so->castPtr("QObject")));
    else {
      QMetaType::Type type = (QMetaType::Type) QMetaType::type(so->className());
      if (type)
        variant = asQVariantOfType(rvalue, type, false);
      else variant = qVariantFromValue(so->ptr());
    }
  }
    break;
  default:
    error("Converting to QVariant: unhandled R type");
  }
  return variant;
}

/* create a QVariant of specific type by first creating a QVariant
   directly, and then attempting to coerce it to the required type.
*/
QVariant asQVariantOfType(SEXP rvalue, QMetaType::Type type, bool tryDirect)
{
  QVariant ans;
  if (tryDirect) {
    QVariant direct = from_sexp<QVariant>(rvalue);
    if (direct.canConvert((QVariant::Type)type)) { // handles a few cases
      direct.convert((QVariant::Type)type);
      return direct;
    }
  }
  switch(type) {
  case QMetaType::QObjectStar:
    {
      SmokeType type(qt_Smoke, "QObject*");
      ans = qVariantFromValue(ptr_from_sexp<QObject *>(rvalue, type));
    }
    break;
  case QMetaType::QWidgetStar:
    {
      SmokeType type(qt_Smoke, "QWidget*");
      ans = qVariantFromValue(ptr_from_sexp<QWidget *>(rvalue, type));
    }
    break;
  case QMetaType::QCursor:
    ans = QVARIANT_FROM_SEXP(rvalue, QCursor);
    break;
  case QMetaType::QDate:
    ans = QVARIANT_FROM_SEXP(rvalue, QDate);
    break;
  case QMetaType::QSize:
    ans = QVARIANT_FROM_SEXP(rvalue, QSize);
    break;
  case QMetaType::QSizeF:
    ans = QVARIANT_FROM_SEXP(rvalue, QSizeF);
    break;
  case QMetaType::QTime:
    ans = QVARIANT_FROM_SEXP(rvalue, QTime);
    break;
  case QMetaType::QVariantList:
    {
      SmokeType type(qt_Smoke, "QList<QVariant>>");
      ans = QVariant(from_sexp<QList<QVariant> >(rvalue, type));
    }
    break;
  case QMetaType::QPolygon:
    ans = QVARIANT_FROM_SEXP(rvalue, QPolygon);
    break;
  case QMetaType::QColor:
    ans = QVARIANT_FROM_SEXP(rvalue, QColor);
    break;
  case QMetaType::QRectF:
    ans = QVARIANT_FROM_SEXP(rvalue, QRectF);
    break;
  case QMetaType::QRect:
    ans = QVARIANT_FROM_SEXP(rvalue, QRect);
    break;
  case QMetaType::QLine:
    ans = QVARIANT_FROM_SEXP(rvalue, QLine);
    break;
  case QMetaType::QTextLength:
    ans = QVARIANT_FROM_SEXP(rvalue, QTextLength);
    break;
  case QMetaType::QStringList:
    {
      SmokeType type(qt_Smoke, "QStringList");
      ans = QVariant(from_sexp<QStringList>(rvalue, type));
    }
    break;
  case QMetaType::QVariantMap:
    {
      SmokeType type(qt_Smoke, "QMap<QString,QVariant>");
      ans = QVariant(from_sexp<QMap<QString,QVariant> >(rvalue, type));
    }
    break;
#if QT_VERSION >= 0x40500
  case QMetaType::QVariantHash:
    {
      SmokeType type(qt_Smoke, "QHash<QString,QVariant>");
      ans = QVariant(from_sexp<QHash<QString,QVariant> >(rvalue, type));
    }
    break;
#endif
  case QMetaType::QIcon:
    ans = QVARIANT_FROM_SEXP(rvalue, QIcon);
    break;
  case QMetaType::QPen:
    ans = QVARIANT_FROM_SEXP(rvalue, QPen);
    break;
  case QMetaType::QLineF:
    ans = QVARIANT_FROM_SEXP(rvalue, QLineF);
    break;
  case QMetaType::QTextFormat:
    ans = QVARIANT_FROM_SEXP(rvalue, QTextFormat);
    break;
  case QMetaType::QPoint:
    ans = QVARIANT_FROM_SEXP(rvalue, QPoint);
  case QMetaType::QPointF:
    ans = QVARIANT_FROM_SEXP(rvalue, QPointF);
    break;
  case QMetaType::QUrl:
    ans = QVARIANT_FROM_SEXP(rvalue, QUrl);
    break;
#if QT_VERSION >= 0x40100
  case QMetaType::QRegExp:
    ans = QVARIANT_FROM_SEXP(rvalue, QRegExp);
    break;
#endif
  case QMetaType::QDateTime:
    ans = QVARIANT_FROM_SEXP(rvalue, QDateTime);
    break;
  case QMetaType::QPalette:
    ans = QVARIANT_FROM_SEXP(rvalue, QPalette);
    break;
  case QMetaType::QFont:
    ans = QVARIANT_FROM_SEXP(rvalue, QFont);
    break;
  case QMetaType::QBrush:
    ans = QVARIANT_FROM_SEXP(rvalue, QBrush);
    break;
  case QMetaType::QRegion:
    ans = QVARIANT_FROM_SEXP(rvalue, QRegion);
    break;
  case QMetaType::QBitArray:
    ans = QVARIANT_FROM_SEXP(rvalue, QBitArray);
    break;
  case QMetaType::QImage:
    ans = QVARIANT_FROM_SEXP(rvalue, QImage);
    break;
  case QMetaType::QKeySequence:
    ans = QVARIANT_FROM_SEXP(rvalue, QKeySequence);
    break;
  case QMetaType::QSizePolicy:
    ans = QVARIANT_FROM_SEXP(rvalue, QSizePolicy);
    break;
  case QMetaType::QPixmap:
    ans = QVARIANT_FROM_SEXP(rvalue, QPixmap);
    break;
  case QMetaType::QLocale:
    ans = QVARIANT_FROM_SEXP(rvalue, QLocale);
    break;
  case QMetaType::QBitmap:
    ans = QVARIANT_FROM_SEXP(rvalue, QBitmap);
    break;
  case QMetaType::QMatrix:
    ans = QVARIANT_FROM_SEXP(rvalue, QMatrix);
    break;
#if QT_VERSION >= 0x40300
  case QMetaType::QTransform:
    ans = QVARIANT_FROM_SEXP(rvalue, QTransform);
    break;
#endif
#if QT_VERSION >= 0x40600
  case QMetaType::QMatrix4x4:
    ans = QVARIANT_FROM_SEXP(rvalue, QMatrix4x4);
    break;
  case QMetaType::QVector2D:
    ans = QVARIANT_FROM_SEXP(rvalue, QVector2D);
    break;
  case QMetaType::QVector3D:
    ans = QVARIANT_FROM_SEXP(rvalue, QVector3D);
    break;
  case QMetaType::QVector4D:
    ans = QVARIANT_FROM_SEXP(rvalue, QVector4D);
    break;
  case QMetaType::QQuaternion:
    ans = QVARIANT_FROM_SEXP(rvalue, QQuaternion);
    break;
#endif
  case QMetaType::User:
    break;
  default:
    error("Converting to QVariant: unhandled Qt type");
  }
  if (!ans.isValid())
    error("Converting to QVariant: Qt type not yet implemented");
  return ans;
}

template<> QByteArray
from_sexp<QByteArray>(SEXP sexp, const SmokeType &type) {
  if (sexp == R_NilValue)
    return QByteArray(0);
  int len = 0;
  const char *data = NULL;
  if (TYPEOF(sexp) == STRSXP) { // do the equivalent of charToRaw()
    if (length(sexp) != 1)
      error("character vector must have length 1 for conversion to QByteArray");
    sexp = STRING_ELT(sexp, 0);
    data = CHAR(sexp);
  } else {
    sexp = coerceVector(sexp, RAWSXP);
    data = (const char *)RAW(sexp);
  }
  return QByteArray(data, length(sexp));
}

/* marked 'static' because we want a different implementation for the
   type handlers (to an externalptr, instead of raw) */
SEXP to_sexp(QByteArray s) {
  if (s.isNull())
    return R_NilValue;
  SEXP sexp = allocVector(RAWSXP, s.size());
  const char *data = s.constData();
  for (int i = 0; i < s.size(); i++)
    RAW(sexp)[i] = data[i];
  return sexp;
}

SEXP to_sexp(QList<QString> list) {
  SEXP vector;
  PROTECT(vector = allocVector(STRSXP, list.size()));
  for(int i = 0; i < length(vector); ++i )
    SET_STRING_ELT(vector, i, mkChar(list.at(i).toLocal8Bit().data()));
  UNPROTECT(1);
  return vector;
}
template<> QList<QString> from_sexp<QList<QString> >(SEXP vector) {
  QList<QString> list;
  for(int i = 0; i < length(vector); i++)
    list.append(from_sexp<QString>(STRING_ELT(vector, i)));
  return list;
}

template<> QStringList from_sexp<QStringList>(SEXP vector) {
  return QStringList(from_sexp<QList<QString> >(vector));
}

SEXP to_sexp(QRectF rect) {
  SEXP rrect = allocMatrix(REALSXP, 2, 2);
  REAL(rrect)[0] = rect.left();
  REAL(rrect)[1] = rect.right();
  REAL(rrect)[2] = rect.top();
  REAL(rrect)[3] = rect.bottom();
  return rrect;
}
template<> QRectF from_sexp<QRectF>(SEXP r) {
  double *rrect = REAL(r);
  return QRectF(QPointF(rrect[0], rrect[2]), QPointF(rrect[1], rrect[3]));
}

SEXP to_sexp(QRect rect) {
  SEXP rrect = allocMatrix(INTSXP, 2, 2);
  INTEGER(rrect)[0] = rect.left();
  INTEGER(rrect)[1] = rect.right();
  INTEGER(rrect)[2] = rect.top();
  INTEGER(rrect)[3] = rect.bottom();
  return rrect;
}

SEXP to_sexp(QTransform tform) {
  SEXP ans = allocMatrix(REALSXP, 3, 3);; 
  REAL(ans)[0] = tform.m11();
  REAL(ans)[1] = tform.m21();
  REAL(ans)[2] = tform.m31();
  REAL(ans)[3] = tform.m12();
  REAL(ans)[4] = tform.m22();
  REAL(ans)[5] = tform.m32();
  REAL(ans)[6] = tform.m13();
  REAL(ans)[7] = tform.m23();
  REAL(ans)[8] = tform.m33();
  return ans;
}

template<> QTransform from_sexp<QTransform>(SEXP m) {
  double *rmatrix = REAL(m);
  return QTransform(rmatrix[0], rmatrix[3], rmatrix[6], rmatrix[1], rmatrix[4],
                    rmatrix[7], rmatrix[2], rmatrix[5], rmatrix[8]);
}

SEXP to_sexp(QPointF point) {
  SEXP rpoint = allocVector(REALSXP, 2);
  REAL(rpoint)[0] = point.x(); REAL(rpoint)[1] = point.y();
  return rpoint;
}

template<> QPointF from_sexp<QPointF>(SEXP p) {
  double *rpoint = REAL(p);
  return QPointF(rpoint[0], rpoint[1]);
}

SEXP to_sexp(QPoint point) {
  SEXP rpoint = allocVector(INTSXP, 2);
  INTEGER(rpoint)[0] = point.x(); INTEGER(rpoint)[1] = point.y();
  return rpoint;
}

SEXP to_sexp(QPolygonF polygon) {
  SEXP rpolygon = allocMatrix(REALSXP, polygon.size(), 2);
  int nr = nrows(rpolygon);
  for (int i = 0; i < nr; i++) {
    REAL(rpolygon)[i] = polygon[i].x();
    REAL(rpolygon)[i + nr] = polygon[i].y();
  }
  return rpolygon;
}

SEXP to_sexp(QPolygon polygon) {
  SEXP rpolygon = allocMatrix(INTSXP, polygon.size(), 2);
  int nr = nrows(rpolygon);
  for (int i = 0; i < nr; i++) {
    INTEGER(rpolygon)[i] = polygon[i].x();
    INTEGER(rpolygon)[i + nr] = polygon[i].y();
  }
  return rpolygon;
}

SEXP to_sexp(QSizeF size) {
  SEXP rsize = allocVector(REALSXP, 2);
  REAL(rsize)[0] = size.width(); REAL(rsize)[1] = size.height();
  return rsize;
}

template<> QSizeF from_sexp<QSizeF>(SEXP s) {
  double *rsize = REAL(s);
  return QSizeF(rsize[0], rsize[1]);
}

SEXP to_sexp(QSize size) {
  SEXP rsize = allocVector(INTSXP, 2);
  INTEGER(rsize)[0] = size.width(); INTEGER(rsize)[1] = size.height();
  return rsize;
}

template<> QColor from_sexp<QColor>(SEXP c) {
  int *rcolor = INTEGER(c);
  return QColor(rcolor[0], rcolor[1], rcolor[2], rcolor[3]);
}
SEXP to_sexp(QColor color) {
  SEXP rcolor = allocMatrix(INTSXP, 4, 1);
  int *rptr = INTEGER(rcolor);
  rptr[0] = color.red();
  rptr[1] = color.green();
  rptr[2] = color.blue();
  rptr[3] = color.alpha();
  return rcolor;
}

/********************* .Call entry point definitions *******************/


#define DEF_COERCE_ENTRY_POINT(type)                     \
  SEXP qt_coerce_##type(SEXP sexp) {                     \
    return to_sexp(*unwrapSmoke(sexp, type));            \
  }

DEF_COERCE_ENTRY_POINT(QRectF)
DEF_COERCE_ENTRY_POINT(QRect)
DEF_COERCE_ENTRY_POINT(QTransform)
DEF_COERCE_ENTRY_POINT(QPointF)
DEF_COERCE_ENTRY_POINT(QPoint)
DEF_COERCE_ENTRY_POINT(QPolygon)
DEF_COERCE_ENTRY_POINT(QPolygonF)
DEF_COERCE_ENTRY_POINT(QSizeF)
DEF_COERCE_ENTRY_POINT(QSize)
DEF_COERCE_ENTRY_POINT(QColor)

