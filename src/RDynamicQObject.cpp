#include "RDynamicQObject.hpp"
#include "MocStack.hpp"
#include "SmokeStack.hpp"
#include "RMethod.hpp"
#include "SmokeType.hpp"

#include <Rinternals.h>

RDynamicQObject::RDynamicQObject(const MocMethod &method, SEXP function,
                                 SEXP userData, QObject *sender)
  : DynamicQObject(sender), _method(method), _function(function),
    _userData(userData)
{
  R_PreserveObject(function);
  if (userData)
    R_PreserveObject(userData);
}
RDynamicQObject::~RDynamicQObject() {
  R_ReleaseObject(_function);
  if (_userData)
    R_ReleaseObject(_userData);
}

DynamicSlot *RDynamicQObject::createSlot(const char *slot) {
  Q_UNUSED(slot);
  return new RDynamicSlot(this);
}

void RDynamicSlot::call(QObject *sender, void **arguments) {
  Q_UNUSED(sender);
  SEXP user_data = object->userData(), function = object->function();
  bool have_user_data = user_data != NULL;
  MocMethod method = object->method();
  // TODO: support the user_data
  QVector<SmokeType> stackTypes = method.types();
  MocStack mocStack = MocStack(arguments, stackTypes.size());
  SmokeStack smokeStack = mocStack.toSmoke(stackTypes);
  RMethod rmethod(NULL, NULL, function, stackTypes);
  rmethod.invoke(NULL, smokeStack.items());
  mocStack.returnFromSmoke(smokeStack, stackTypes[0]);
}
