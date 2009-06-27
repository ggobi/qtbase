/* Get and set properties */

#include <QObject>
#include <QMetaObject>
#include <QMetaProperty>
#include <QVariant>

#include "wrappers.h"
#include "utils.hpp"

SEXP asRVariant(QVariant variant) {
  SEXP ans = NULL;
  switch(variant.type()) {
  case QMetaType::Void:
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
    ans = wrapPointer(variant.value<void *>(), NULL, NULL);
    break;
  case QMetaType::QObjectStar:
    ans = wrapQObject(variant.value<QObject *>());
    break;
  case QMetaType::QWidgetStar:
    ans = wrapQWidget(variant.value<QWidget *>());
    break;
  case QMetaType::QCursor:
    break;
  case QMetaType::QDate:
    break;
  case QMetaType::QSize:
    break;
  case QMetaType::QTime:
    break;
  case QMetaType::QVariantList:
    break;
  case QMetaType::QPolygon:
    break;
  case QMetaType::QColor:
    break;
  case QMetaType::QSizeF:
    break;
  case QMetaType::QRectF:
    break;
  case QMetaType::QLine:
    break;
  case QMetaType::QTextLength:
    break;
  case QMetaType::QStringList:
    break;
  case QMetaType::QVariantMap:
    break;
  case QMetaType::QVariantHash:
    break;
  case QMetaType::QIcon:
    break;
  case QMetaType::QPen:
    break;
  case QMetaType::QLineF:
    break;
  case QMetaType::QTextFormat:
    break;
  case QMetaType::QRect:
    break;
  case QMetaType::QPoint:
    break;
  case QMetaType::QUrl:
    break;
  case QMetaType::QRegExp:
    break;
  case QMetaType::QDateTime:
    break;
  case QMetaType::QPointF:
    break;
  case QMetaType::QPalette:
    break;
  case QMetaType::QFont:
    break;
  case QMetaType::QBrush:
    break;
  case QMetaType::QRegion:
    break;
  case QMetaType::QBitArray:
    break;
  case QMetaType::QImage:
    break;
  case QMetaType::QKeySequence:
    break;
  case QMetaType::QSizePolicy:
    break;
  case QMetaType::QPixmap:
    break;
  case QMetaType::QLocale:
    break;
  case QMetaType::QBitmap:
    break;
  case QMetaType::QMatrix:
    break;
  case QMetaType::QTransform:
    break;
  case QMetaType::User:
    break;
  default:
    error("Converting from QVariant: unhandled Qt type");
  }
  if (!ans)
    error("Converting from QVariant: Qt type not yet implemented");
  return ans;
}

extern "C" SEXP qt_qproperty(SEXP x, SEXP name) {
  QObject *obj = unwrapQObject(x, QObject);
  QVariant variant = obj->property(CHAR(asChar(name)));
  return asRVariant(variant);
}

/* create a QVariant directly from an R object */
static QVariant asQVariant(SEXP rvalue) {
  QVariant variant;
  switch(TYPEOF(rvalue)) {
  case LGLSXP:
    variant = QVariant(asLogical(rvalue));
    break;
  case REALSXP:
    variant = QVariant(asReal(rvalue));
    break;
  case INTSXP:
    variant = QVariant(asInteger(rvalue));
    break;
  case STRSXP:
    variant = QVariant(sexp2qstring(rvalue));
    break;
  case EXTPTRSXP:
    variant = qVariantFromValue(unwrapPointer(rvalue, void));
    break;
  default:
    error("Converting to QVariant: unhandled R type");
  }
  return variant;
}

/* create a QVariant of specific type by first creating a QVariant
   directly, and then attempting to coerce it to the required type.
*/
static QVariant asQVariantOfType(SEXP rvalue, QMetaType::Type type) {
  QVariant direct = asQVariant(rvalue);
  QVariant ans;
  if (direct.canConvert((QVariant::Type)type)) // handles a few cases
    return direct.convert((QVariant::Type)type);
  switch(type) {
    // FIXME: should verify that these pointers are of right R class
  case QMetaType::QObjectStar:
    ans = qVariantFromValue(unwrapQObject(rvalue, QObject));
    break;
  case QMetaType::QWidgetStar:
    ans = qVariantFromValue(unwrapQObject(rvalue, QWidget));
    break;
  case QMetaType::QCursor:
    break;
  case QMetaType::QDate:
    break;
  case QMetaType::QSize:
    break;
  case QMetaType::QTime:
    break;
  case QMetaType::QVariantList:
    break;
  case QMetaType::QPolygon:
    break;
  case QMetaType::QColor:
    break;
  case QMetaType::QSizeF:
    break;
  case QMetaType::QRectF:
    break;
  case QMetaType::QLine:
    break;
  case QMetaType::QTextLength:
    break;
  case QMetaType::QStringList:
    break;
  case QMetaType::QVariantMap:
    break;
  case QMetaType::QVariantHash:
    break;
  case QMetaType::QIcon:
    break;
  case QMetaType::QPen:
    break;
  case QMetaType::QLineF:
    break;
  case QMetaType::QTextFormat:
    break;
  case QMetaType::QRect:
    break;
  case QMetaType::QPoint:
    break;
  case QMetaType::QUrl:
    break;
  case QMetaType::QRegExp:
    break;
  case QMetaType::QDateTime:
    break;
  case QMetaType::QPointF:
    break;
  case QMetaType::QPalette:
    break;
  case QMetaType::QFont:
    break;
  case QMetaType::QBrush:
    break;
  case QMetaType::QRegion:
    break;
  case QMetaType::QBitArray:
    break;
  case QMetaType::QImage:
    break;
  case QMetaType::QKeySequence:
    break;
  case QMetaType::QSizePolicy:
    break;
  case QMetaType::QPixmap:
    break;
  case QMetaType::QLocale:
    break;
  case QMetaType::QBitmap:
    break;
  case QMetaType::QMatrix:
    break;
  case QMetaType::QTransform:
    break;
  case QMetaType::User:
    break;
  default:
    error("Converting to QVariant: unhandled Qt type");
  }
  if (!ans.isValid())
    error("Converting to QVariant: Qt type not yet implemented");
  return ans;
}

extern "C" SEXP qt_qsetProperty(SEXP x, SEXP rname, SEXP rvalue) {
  QObject *obj = unwrapQObject(x, QObject);
  const QMetaObject *meta = obj->metaObject();
  const char *name = CHAR(asChar(rname));
  QMetaProperty prop = meta->property(meta->indexOfProperty(name));
  QVariant variant = asQVariantOfType(rvalue, (QMetaType::Type)prop.userType());
  prop.write(obj, variant);
  return x;
}
