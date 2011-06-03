#include <QObject>

#include "MocDynamicBinding.hpp"
#include "MocStack.hpp"
#include "SmokeStack.hpp"
#include "SmokeObject.hpp"
#include "SmokeType.hpp"
#include "MocMethod.hpp"

MocDynamicBinding::MocDynamicBinding(const MocMethod &method)
  : DynamicBinding(method) { }

void MocDynamicBinding::invoke(QObject *obj, void **stack) {
  QVector<SmokeType> stackTypes = types();
  MocStack mocStack = MocStack(stack, stackTypes.size());
  SmokeStack smokeStack = mocStack.toSmoke(stackTypes);
  SmokeObject *o = SmokeObject::fromPtr(obj, klass());
  DynamicBinding::invoke(o, smokeStack.items());
  mocStack.returnFromSmoke(smokeStack, stackTypes[0]);
}
