/* Get and set properties */

#include <QObject>
#include <QMetaObject>
#include <QMetaProperty>
#include <QVariant>

#include "convert.hpp"

static QMetaProperty getMetaProperty(QObject *obj, const char *name) {
  const QMetaObject *meta = obj->metaObject();
  int index = meta->indexOfProperty(name);
  if (index == -1)
    error("property does not exist");
  return meta->property(index);
}
  
extern "C" SEXP qt_qproperty(SEXP x, SEXP rname) {
  QObject *obj = unwrapQObject(x, QObject);
  QMetaProperty prop = getMetaProperty(obj, CHAR(asChar(rname)));
  if (!prop.isReadable())
    error("property is not readable");
  QVariant variant = prop.read(obj);
  return asRVariant(variant);
}



extern "C" SEXP qt_qsetProperty(SEXP x, SEXP rname, SEXP rvalue) {
  QObject *obj = unwrapQObject(x, QObject);
  QMetaProperty prop = getMetaProperty(obj, CHAR(asChar(rname)));
  if (!prop.isWritable())
    error("property is not writable");
  QVariant variant = asQVariantOfType(rvalue, (QMetaType::Type)prop.userType());
  prop.write(obj, variant);
  return x;
}
