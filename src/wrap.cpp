#include "SmokeObject.hpp"
#include "Class.hpp"
#include "wrap.hpp"

#include <Rdefines.h>

void *_unwrapSmoke(SEXP x, const char *type) {
  void *ans = NULL;
  SmokeObject *so = SmokeObject::fromSexp(x);
  if (so)
    ans = so->castPtr(type);
  return ans;
}

/* We do not share Smoke objects across packages, so we look them up
   via 'className'. We could accept the type name, rather than the
   class name, but there is no map to a Smoke module. That would have
   allowed us to guess 'allocated', but we just default that to 'true'
   (works in most cases). We fix 'copy' to 'false', since manual
   bindings should not need to use reflection for copying. */
SEXP _wrapSmoke(void *ptr, const char *className, bool allocated)
{
  return SmokeObject::sexpFromPtr(ptr, Class::fromName(className), allocated);
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
