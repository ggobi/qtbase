/* Get and set properties */

#include <QObject>
#include <QMetaObject>
#include <QMetaProperty>
#include <QVariant>

#include "convert.hpp"


extern "C" SEXP qt_qproperty(SEXP x, SEXP name) {
  QObject *obj = unwrapQObject(x, QObject);
  QVariant variant = obj->property(CHAR(asChar(name)));
  return asRVariant(variant);
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
