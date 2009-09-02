#include "RClass.hpp"
#include "RMethod.hpp"
#include "MethodCall.hpp"

#include <Rinternals.h>

const char *RClass::name() const {
  static SEXP nameSym = install("name");
  return CHAR(asChar(getAttrib(_klass, nameSym)));
}

SEXP RClass::env() const {
  static SEXP envSym = install("env");
  return getAttrib(_klass, envSym);
}

const Class* RClass::parent() const {
  static SEXP parentSym = install("parent");
  return Class::fromSexp(getAttrib(_klass, parentSym));
}

QList<const Class *> RClass::parents() const {
  QList<const Class *> classes;
  classes.append(parent());
  return classes;
}

const SmokeClass *RClass::smokeBase() const {
  return parent()->smokeBase();
}
  
QList<Method *> RClass::methods() const {
  SEXP _env = env();
  SEXP names = R_lsInternal(_env, (Rboolean)false);
  QList<Method *> meths;
  for (int i = 0; i < length(names); i++) {
    const char *methodName = CHAR(STRING_ELT(names, i));
    meths << new RMethod(this, methodName, findFun(_env, install(methodName)));
  }
  return meths;
}

Method *RClass::findMethod(const MethodCall &call) {
  const char * methodName = call.method()->name();
  return new RMethod(this, methodName, findFun(env(), install(methodName)));
}
