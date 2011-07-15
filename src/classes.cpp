#include "SmokeObject.hpp"
#include "Class.hpp"

#include <Rinternals.h>

extern "C"
SEXP qt_qcast(SEXP x, SEXP klass) {
  SmokeObject *obj = SmokeObject::fromSexp(x);
  obj->cast(Class::fromSexp(klass));
  return obj->sexp();
}

extern "C"
SEXP qt_qenclose(SEXP x, SEXP fun) {
  return SmokeObject::fromSexp(x)->enclose(fun);
}

extern "C"
SEXP qt_qinitClass(SEXP x) {
  Class::fromSexp(x, true);
  return R_NilValue;
}
