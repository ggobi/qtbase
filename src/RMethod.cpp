#include "RMethod.hpp"
#include "MethodCall.hpp"
#include "SmokeObject.hpp"

#include <Rinternals.h>

SEXP RMethod::invoke(SEXP self, SEXP args) {
  SEXP lang, lang_tmp, fun;
  int problem = 0;
  
  PROTECT(lang = allocVector(LANGSXP, length(args) + 1 + (_userData != NULL)));
  
  if (self) {
    SmokeObject *so = SmokeObject::fromSexp(self);
    fun = so->enclose(_closure);
  } else fun = _closure;
  SETCAR(lang, fun);
  
  lang_tmp = CDR(lang);
  for (int i = 0; i < length(args); i++) {
    SETCAR(lang_tmp, VECTOR_ELT(args, i));
    lang_tmp = CDR(lang_tmp);
  }
  if (_userData)
    SETCAR(lang_tmp, _userData);
  SEXP ans = R_tryEval(lang, R_GlobalEnv, &problem);
  UNPROTECT(1);
  if (problem)
    setLastError(ImplementationFailed);
  else setLastError(NoError);
  return ans;
}

#undef eval // work around surprising C++ feature
void RMethod::invoke(SmokeObject *obj, Smoke::Stack stack) {
  MethodCall call(this, obj, stack);
  call.eval();
}

Method::Qualifiers RMethod::qualifiers() const {
  static SEXP accessSym = install("access");
  static SEXP staticSym = install("static");
  Qualifiers qual;
  SEXP staticAttr = getAttrib(_closure, staticSym);
  if (staticAttr != R_NilValue && asLogical(staticAttr))
    qual |= Static;
  else qual |= NotStatic;
  const char *access = CHAR(asChar(getAttrib(_closure, accessSym)));
  if (!qstrcmp(access, "private"))
    qual |= Private;
  else {
    qual |= NotPrivate;
    if (!qstrcmp(access, "protected"))
      qual |= Protected;
    else qual |= Public;
  }
  return qual;
}
