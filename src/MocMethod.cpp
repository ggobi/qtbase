#include "MocMethod.hpp"
#include "MocStack.hpp"
#include "SmokeStack.hpp"
#include "SmokeObject.hpp"
#include "SmokeList.hpp"
#include "SmokeType.hpp"
#include "SmokeModule.hpp"
#include "Class.hpp"

MocMethod::MocMethod(Smoke *smoke, const QMetaObject *meta, int id)
  : _method(meta->method(id)), _meta(meta), _id(id), _smoke(smoke)
{
  QByteArray signature(_method.signature());
  _name = signature.mid(0, signature.indexOf('('));
}

void MocMethod::invoke(SmokeObject *o, Smoke::Stack stack) {
  QVector<SmokeType> _types = types();
  SmokeStack smokeStack = SmokeStack(stack, _types.size());
  MocStack mocStack = MocStack(smokeStack, _types);
  invoke(reinterpret_cast<QObject *>(o->castPtr("QObject")), mocStack.items());
  mocStack.returnToSmoke(smokeStack, _types[0]);
  // TODO: if this is a constructor, need to set SmokeBinding
}

void MocMethod::invoke(QObject *obj, void **o) {
  setLastError(NoError);
  if (!obj->qt_metacall(QMetaObject::InvokeMetaMethod, _id, o)) {
    qCritical("Failed to invoke metamethod %s::%s\n", _meta->className(),
              name());
    setLastError(InvocationFailed);
  }
}

QVector<SmokeType> MocMethod::types() const {
  int i = 0;
  QList<QByteArray> methodTypes = _method.parameterTypes();
  const char *className = _meta->className();
  QVector<SmokeType> _types(methodTypes.size() + 1);
  methodTypes.prepend(QByteArray(_method.typeName()));
  foreach (QByteArray name, methodTypes) {
    Smoke *smoke = _smoke;
    SmokeType type;
    if (!name.isEmpty()) { // should only be empty for void (return)
      type = SmokeType(smoke, name, className);
      // Yes, slot arguments can come from different smoke modules
      if (type.isVoid()) {
        SmokeList smokes = SmokeModule::smokes();
        foreach(smoke, smokes) {
          type = SmokeType(smoke, name, className);
          if (!type.isVoid())
            break;
        }
      }
      if (type.isVoid()) {
        qCritical("Cannot handle Moc type '%s'\n", name.constData());
      }
    }
    _types[i++] = type;
  }
  return _types;
}

const Class* MocMethod::klass() const {
  return Class::fromSmokeName(_smoke, _meta->className());
}
