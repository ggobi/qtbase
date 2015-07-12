#include "MocProperty.hpp"
#include "MocStack.hpp"
#include "SmokeStack.hpp"
#include "DynamicBinding.hpp"

#include "convert.hpp"

SEXP MocProperty::read(SEXP obj) const {
  QObject *qobj = unwrapSmoke(obj, QObject);
  return to_sexp(qobj->property(name()));
}

Smoke::StackItem MocProperty::stackItemFromQVariant(QVariant variant, Smoke *s)
  const
{
  void *ptr = const_cast<void *>(variant.constData()); // undocumented
  MocStack mocStack(&ptr, 1);
  QVector<SmokeType> types;
  types += SmokeType(s, variant.typeName());
  return mocStack.toSmoke(types).ret();
}

QVariant MocProperty::stackItemToQVariant(const Smoke::StackItem &item,
                                          Smoke *s) const
{
  QVector<SmokeType> types;
  types += SmokeType(s, _property.typeName());
  MocStack mocStack(SmokeStack(const_cast<Smoke::Stack>(&item), 1), types);
  return QVariant(_property.type(), mocStack.items()[0]);
}

/* We could either call QObject::property and convert between QVariant
   and the Smoke::Stack (which is not yet implemented) or convert from
   Smoke::Stack to the MocStack and use QObject::qt_metacall(). The
   latter is faster, easier and consistent with MocMethod, although
   one worries about depending on Qt internals. An alternative is to
   call MetaProperty::read(), get the QVariant, and put the const-data of the
   QVariant on a pretend Moc stack for conversion. */
Smoke::StackItem MocProperty::read(SmokeObject *so) const {
  QObject *qobj = reinterpret_cast<QObject *>(so->castPtr("QObject"));
  return stackItemFromQVariant(_property.read(qobj), so->smoke());
}

bool MocProperty::write(SEXP obj, SEXP val) {
  QObject *qobj = unwrapSmoke(obj, QObject);
  return qobj->setProperty(name(), from_sexp<QVariant>(val));
}

bool MocProperty::write(SmokeObject *so, const Smoke::StackItem &item) {
  QObject *qobj = reinterpret_cast<QObject *>(so->castPtr("QObject"));
  return _property.write(qobj, stackItemToQVariant(item, so->smoke()));
}
