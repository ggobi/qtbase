/* experimental support for dynamic invocation of methods */

#include "DynamicBinding.hpp"
#include "Class.hpp"
#include "SmokeMethod.hpp"

#include <Rinternals.h>

static void reportMethodError(const Method &binding,
                               const char *className);

extern "C"
SEXP qt_qinvoke(SEXP self, SEXP method, SEXP super, SEXP args) {
  const char * methodName = CHAR(asChar(method));
  bool sup = asLogical(super);
  DynamicBinding binding(methodName, sup);
  SEXP ans = binding.invoke(self, args);
  if (binding.lastError() > Method::NoError)
    reportMethodError(binding, CHAR(asChar(getAttrib(self, R_ClassSymbol))));
  return ans;
}

extern "C"
SEXP qt_qinvokeStatic(SEXP rklass, SEXP method, SEXP args) {
  const char * methodName = CHAR(asChar(method));
  const Class *klass = Class::fromSexp(rklass);
  DynamicBinding binding(klass, methodName);
  SEXP ans = binding.invoke(NULL, args);
  if (binding.lastError() > Method::NoError)
    reportMethodError(binding, klass->name());  
  return ans;
}

extern "C" SEXP invokeSmokeMethod(Smoke::ModuleIndex m, SEXP x, SEXP args) {
  SmokeMethod method(m);
  SEXP ans = method.invoke(x, args);
  if (method.lastError() > Method::NoError)
    reportMethodError(method, method.klass()->name());  
  return ans;
}

static void reportMethodError(const Method &method, const char *className)
{
  Method::ErrorType err = method.lastError();
  const char *methodName = method.name();
  if (err == Method::ImplementationMissing)
    error("Could not find method named '%s::%s'", className, methodName);
  else if (err == Method::BadArguments)
    error("Wrong number or types of arguments passed to '%s::%s'", className,
          methodName);
  else if (err == Method::ImplementationFailed)
    error("Implementation failed for method '%s::%s'", className, methodName);
  else if (err > Method::NoError)
    error("Invocation failed for method '%s::%s'", className, methodName);
}
