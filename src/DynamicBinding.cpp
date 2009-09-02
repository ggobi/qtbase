#include "DynamicBinding.hpp"
#include "MethodCall.hpp"
#include "Class.hpp"

SEXP DynamicBinding::invoke(SEXP obj, SEXP args) {
  SEXP ans = NULL;
  MethodCall call(this, obj, args);
  Method *method = call.klass()->findMethod(call);
  if (method) {
    ans = method->invoke(obj, args);
    setLastError(method->lastError());
    delete method;
  } else setLastError(ImplementationMissing);
  return ans;  
}

void DynamicBinding::invoke(SmokeObject *obj, Smoke::Stack args) {
  MethodCall call(this, obj, args);
  Method *method = call.klass()->findMethod(call);
  if (method) {
    method->invoke(obj, args);
    setLastError(method->lastError());
    delete method;
  } else setLastError(ImplementationMissing);
}

