#include "ForeignMethod.hpp"
#include "MethodCall.hpp"

SEXP ForeignMethod::invoke(SEXP obj, SEXP args) {
  MethodCall call = MethodCall(this, obj, args);
  call.eval();
  return call.sexp();
}

