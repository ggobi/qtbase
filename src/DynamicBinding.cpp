#include "DynamicBinding.hpp"
#include "MethodCall.hpp"
#include "Class.hpp"
  
SEXP DynamicBinding::invoke(SEXP obj, SEXP args) {
  SEXP ans = NULL;
  MethodCall call(this, obj, args, _super);
  Method *method = call.klass()->findMethod(call);
  if (method) {
    ans = method->invoke(obj, args);
    setLastError(method->lastError());
    delete method;
  } else setLastError(methodNotFound(call));
  return ans;
}

void DynamicBinding::invoke(SmokeObject *obj, Smoke::Stack args) {
  MethodCall call(this, obj, args, _super);
  Method *method = call.klass()->findMethod(call);
  if (method) {
    method->invoke(obj, args);
    setLastError(method->lastError());
    delete method;
  } else setLastError(methodNotFound(call));
}

Method::ErrorType DynamicBinding::methodNotFound(const MethodCall &call) {
  if (!call.klass()->hasMethod(call.method()->name()))
    return ImplementationMissing;
  return BadArguments;
}
