#include "RProperty.hpp"

#include "SmokeObject.hpp"
#include "SmokeType.hpp"
#include "RMethod.hpp"

#include <Rinternals.h>

/* An R property is defined by a name and a pair of closures for
   accessing the data. Could/should the R function be a method?
   Probably not. The R interface favors accessing properties directly,
   rather than calling accessor functions. This leads to conflicts
   that can be confusing. Thus, we should strive to avoid explicit
   accessors. This is in line with R reference classes. */

SEXP RProperty::read(SEXP obj) const {
  SmokeObject *so = SmokeObject::fromSexp(obj);
  SEXP args, ans = R_NilValue;
  PROTECT(args = allocVector(VECSXP, 0));
  ans = RMethod(so->klass(), _name + "::reader", _reader).invoke(obj, args);
  UNPROTECT(1);
  return ans;
}

Smoke::StackItem RProperty::read(SmokeObject *so) const {
  Smoke::StackItem item;
  QVector<SmokeType> types;
  types += _type;
  RMethod(so->klass(), _name + "::reader", _reader, types).invoke(so, &item);
  return item;
}

bool RProperty::write(SEXP obj, SEXP value) {
  SmokeObject *so = SmokeObject::fromSexp(obj);
  if (!isWritable())
    return false;
  RMethod meth(so->klass(), _name + "::writer", _writer);
  SEXP args;
  PROTECT(args = allocVector(VECSXP, 1));
  SET_VECTOR_ELT(args, 0, value);
  meth.invoke(obj, args);
  UNPROTECT(1);
  return meth.lastError() == Method::NoError;
}

bool RProperty::write(SmokeObject *so, const Smoke::StackItem &item) {
  Smoke::StackItem stack[2];
  stack[1] = item;
  if (!isWritable())
    return false;
  QVector<SmokeType> types;
  types += SmokeType(so->smoke(), (Smoke::Index)0); // void return
  types += _type;
  RMethod meth(so->klass(), _name + "::writer", _writer, types);
  meth.invoke(so, stack);
  return meth.lastError() == Method::NoError;
}

bool RProperty::isReadable() const {
  return true;
}
bool RProperty::isWritable() const {
  return _writer != R_NilValue;
}

/* Could hard-code property access in C++ */

/*
SEXP RProperty::readField(SmokeObject *so) {
  return findVarInFrame(so->fieldEnv(), install(_name));
}

void RProperty::writeField(SmokeObject *so, SEXP val) {
  defineVar(install(_name), val, so->fieldEnv());
}
*/
