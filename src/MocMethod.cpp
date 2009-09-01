#include "MocMethod.hpp"
#include "MocStack.hpp"
#include "SmokeStack.hpp"
#include "SmokeObject.hpp"
#include "SmokeList.hpp"
#include "SmokeType.hpp"
#include "RQtModule.hpp"
#include "Class.hpp"

void MocMethod::invoke(SmokeObject *o, Smoke::Stack stack) {
  QVector<SmokeType> _types = types();
  SmokeStack smokeStack = SmokeStack(stack, _types.size());
  MocStack mocStack = MocStack(smokeStack, _types);
  invoke(reinterpret_cast<QObject *>(o->cast("QObject")), mocStack.items());
  mocStack.returnToSmoke(smokeStack, _types[0]);
}

void MocMethod::invoke(QObject *obj, void **o) {
  setLastError(NoError);
  if (!obj->qt_metacall(QMetaObject::InvokeMetaMethod, _id, o)) {
    qCritical("Failed to invoke metamethod %s::%s\n", _meta->className(),
              name());
    setLastError(InvocationFailed);
  }
}

Smoke::Index
MocMethod::smokeTypeForName(Smoke *smoke, QByteArray name) const {
  Smoke::Index typeId = smoke->idType(name.constData());
  if (typeId == 0 && !name.contains('*')) {
    if (!name.contains("&")) {
      name += "&";
    }
    typeId = smoke->idType(name.constData());
  }
  return typeId;
}

QVector<SmokeType> MocMethod::types() const {
  QList<QByteArray> methodTypes = _method.parameterTypes();
  QVector<SmokeType> _types(methodTypes.size() + 1);
  methodTypes.prepend(QByteArray(_method.typeName()));  
  foreach (QByteArray name, methodTypes) {
    Smoke *smoke = _smoke;
    Smoke::Index typeId = 0;
    if (!name.isEmpty()) { // should only be empty for void (return)
      name.replace("const ", "");
      typeId = smokeTypeForName(smoke, name);
      // Yes, slot arguments can come from different smoke modules
      if (typeId == 0) {
        SmokeList smokes = RQtModule::smokes();
        foreach(smoke, smokes) {
          typeId = smokeTypeForName(smoke, name);
          if (typeId != 0)
            break;
        }
      }
      if (typeId == 0) {
        qCritical("Cannot handle Moc type '%s'\n", name.constData());
      }
    }
    _types.append(SmokeType(smoke, typeId));
  }
  return _types;
}

const Class* MocMethod::klass() const {
  return Class::fromSmokeName(_smoke, _meta->className());
}

