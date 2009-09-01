#include "RMethod.hpp"
#include "MethodCall.hpp"

#include <Rinternals.h>

/* TODO: 'self' may be either an object or NULL (static method).
   Obtain an environment as set as enclosure of our closure. */
SEXP RMethod::invoke(SEXP /*self*/, SEXP args) {
  SEXP lang, lang_tmp;
  int problem;
  PROTECT(lang = allocVector(LANGSXP, length(args) + 2));
  SETCAR(lang, _closure);
  lang_tmp = CDR(lang);
  /*
    SETCAR(lang_tmp, self);
    lang_tmp = CDR(lang_tmp);
  */
  for (int i = 0; i < length(args); i++) {
    SETCAR(lang_tmp, VECTOR_ELT(args, i));
    lang_tmp = CDR(lang_tmp);
  }
  SEXP ans = R_tryEval(lang, R_GlobalEnv, &problem);
  UNPROTECT(1);
  if (problem)
    setLastError(ImplementationFailed);
  return ans;
}

#undef eval // work around surprising C++ feature
void RMethod::invoke(SmokeObject *obj, Smoke::Stack stack) {
  MethodCall call(this, obj, stack);
  call.eval();
}

