/* experimental support for dynamic invocation of methods */

#include "DynamicBinding.hpp"
#include "Class.hpp"

#include <Rinternals.h>

static void reportBindingError(const DynamicBinding &binding,
                               const char *className);

extern "C"
SEXP qt_qinvoke(SEXP self, SEXP method, SEXP args) {
  const char * methodName = CHAR(asChar(method));
  DynamicBinding binding(methodName);
  SEXP ans = binding.invoke(self, args);
  if (binding.lastError() > Method::NoError)
    reportBindingError(binding, CHAR(asChar(getAttrib(self, R_ClassSymbol))));
  return ans;
}

extern "C"
SEXP qt_qinvokeStatic(SEXP rklass, SEXP method, SEXP args) {
  const char * methodName = CHAR(asChar(method));
  const Class *klass = Class::fromSexp(rklass);
  DynamicBinding binding(klass, methodName);
  SEXP ans = binding.invoke(NULL, args);
  if (binding.lastError() > Method::NoError)
    reportBindingError(binding, klass->name());  
  return ans;
}

static void reportBindingError(const DynamicBinding &binding,
                               const char *className)
{
  Method::ErrorType err = binding.lastError();
  const char *methodName = binding.name();
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
