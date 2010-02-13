#include "SmokeObject.hpp"
#include "wrap.hpp"

#include <Rdefines.h>

void *_unwrapSmoke(SEXP x, const char *type) {
  void *ans = NULL;
  SmokeObject *so = SmokeObject::fromSexp(x);
  if (so)
    ans = so->castPtr(type);
  return ans;
}

SEXP wrapPointer(void *ptr, QList<QByteArray> classNames,
                   R_CFinalizer_t finalizer)
{
  SEXP ans;
  PROTECT(ans = R_MakeExternalPtr(ptr, R_NilValue, R_NilValue));
  if (finalizer)
    R_RegisterCFinalizer(ans, finalizer);
  SEXP rclassNames = allocVector(STRSXP, classNames.size());
  SET_CLASS(ans, rclassNames);
  for (int i = 0; i < length(rclassNames); i++)
    SET_STRING_ELT(rclassNames, i, mkChar(classNames[i].constData()));
  UNPROTECT(1);
  return ans;
}
