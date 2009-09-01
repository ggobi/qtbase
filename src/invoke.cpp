/* experimental support for dynamic invocation of methods */

#include "DynamicBinding.hpp"
#include "Class.hpp"

#include <Rinternals.h>

SEXP qt_qinvoke(SEXP method, SEXP self, SEXP args) {
  const char * methodName = CHAR(asChar(method));
  DynamicBinding binding(methodName);
  SEXP ans = binding.invoke(self, args);
  if (binding.lastError() > Method::NoError) {
    const char *className = CHAR(asChar(getAttrib(self, R_ClassSymbol)));
    if (binding.lastError() == Method::ImplementationMissing)
      error("Could not find method '%s' for object of class '%s'",
            methodName, className);
    else error("Invocation failed for method '%s' for object of class '%s'",
               methodName, className);
  }
  return ans;
}

SEXP qt_qinvokeStatic(SEXP method, SEXP rklass, SEXP args) {
  const char * methodName = CHAR(asChar(method));
  Class *klass = Class::fromSexp(rklass);
  DynamicBinding binding(klass, methodName);
  SEXP ans = binding.invoke(NULL, args);
  if (binding.lastError() == Method::ImplementationMissing)
    error("Could not find method '%s::%s'", klass->name(), methodName);
  else if (binding.lastError() > Method::NoError)
    error("Invocation failed for method '%s::%s'", klass->name(), methodName);
  return ans;
}

/*
SEXP qt_qinvokeStaticR(SEXP method, SEXP klass, SEXP args) {
  const char * methodName = CHAR(asChar(method));
  RClass rclass = RClass(klass);
  RDynamicBinding binding(methodSelectors, rclass, methodName);
  SEXP ans = binding.invoke(NULL, args);
  if (binding.lastError() == Method::ImplementationMissing)
    error("Could not find method '%s::%s'", rclass.name(), methodName);
  else if (binding.lastError() > Method::NoError)
    error("Invocation failed for method '%s::%s'", rclass.name(), methodName);
  return ans;
}
*/
