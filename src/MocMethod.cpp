#include "MocMethod.hpp"
#include "MocStack.hpp"
#include "SmokeStack.hpp"
#include "SmokeObject.hpp"
#include "SmokeList.hpp"
#include "SmokeType.hpp"
#include "RQtModule.hpp"
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
}

void MocMethod::invoke(QObject *obj, void **o) {
  setLastError(NoError);
  if (!obj->qt_metacall(QMetaObject::InvokeMetaMethod, _id, o)) {
    qCritical("Failed to invoke metamethod %s::%s\n", _meta->className(),
              name());
    setLastError(InvocationFailed);
  }
}

/* Qt normalizes value type names by removing 'const' and '&'; we work
   around that here. */
Smoke::Index MocMethod::smokeTypeForName(Smoke *smoke, QByteArray name) const {
  Smoke::Index typeId = smoke->idType(name);
  if (typeId == 0 && !name.endsWith("*")) {
    QByteArray constName = "const " + name;
    typeId = smoke->idType(constName);
    if (typeId == 0) {
      constName += "&";
      typeId = smoke->idType(constName);
      if (typeId == 0) { // could be an enum or internal class
        QByteArray enumName = _meta->className() + ("::" + name);
        typeId = smoke->idType(enumName);
        if (typeId == 0) { // more special cases
          if (name == "qint64")
            typeId = smoke->idType("long long");
        }
      }
    }
  }
  return typeId;
}

QVector<SmokeType> MocMethod::types() const {
  int i = 0;
  QList<QByteArray> methodTypes = _method.parameterTypes();
  QVector<SmokeType> _types(methodTypes.size() + 1);
  methodTypes.prepend(QByteArray(_method.typeName()));
  foreach (QByteArray name, methodTypes) {
    Smoke *smoke = _smoke;
    Smoke::Index typeId = 0;
    if (!name.isEmpty()) { // should only be empty for void (return)
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
    _types[i++] = SmokeType(smoke, typeId);
  }
  return _types;
}

const Class* MocMethod::klass() const {
  return Class::fromSmokeName(_smoke, _meta->className());
}
