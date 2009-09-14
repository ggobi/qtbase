#include "MocProperty.hpp"
#include "DynamicBinding.hpp"
#include "SmokeObject.hpp"

#include "convert.hpp"

SEXP MocProperty::read(SEXP obj) const {
  QObject *qobj = unwrapSmoke(obj, QObject);
  return asRVariant(qobj->property(name()));
}

Smoke::StackItem MocProperty::read(SmokeObject *so) const {
  Smoke::StackItem items[2];
  items[1].s_voidp = const_cast<char *>(name());
  so->invokeMethod("property", items);
  return items[0];
}

bool MocProperty::write(SEXP obj, SEXP val) {
  QObject *qobj = unwrapSmoke(obj, QObject);
  return qobj->setProperty(name(), asQVariant(val));
}

bool MocProperty::write(SmokeObject *so, const Smoke::StackItem &item) {
  Smoke::StackItem items[3];
  items[1].s_voidp = const_cast<char *>(name());
  items[2] = item;
  so->invokeMethod("setProperty", items);
  return items[0].s_bool;
}
