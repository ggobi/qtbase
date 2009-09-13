#include "RClass.hpp"
#include "RMethod.hpp"
#include "MethodCall.hpp"
#include "SmokeClass.hpp"

#include <Rinternals.h>

RClass::RClass(SEXP klass) : _klass(klass) {
  R_PreserveObject(klass);
}

RClass::~RClass() {
  R_ReleaseObject(_klass);
}

const char *RClass::name() const {
  static SEXP nameSym = install("name");
  return CHAR(asChar(getAttrib(_klass, nameSym)));
}

SEXP RClass::env() const {
  /* Eventually we might have one 'env' attribute that holds all user
     symbols, with the environment of static symbols becoming dynamic.
  */
  static SEXP envSym = install("instanceEnv");
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

QList<Method *> RClass::methods(Method::Qualifiers qualifiers) const {
  SEXP _env = env();
  SEXP names = R_lsInternal(_env, (Rboolean)false);
  QList<Method *> meths;
  for (int i = 0; i < length(names); i++) {
    const char *name = CHAR(STRING_ELT(names, i));
    SEXP fun = findVarInFrame(_env, install(name));
    if ((RMethod(this, name, fun).qualifiers() & qualifiers) == qualifiers)
      meths << new RMethod(this, name, fun);
  }
  meths.append(parent()->methods(qualifiers | Method::NotPrivate));
  return meths;
}

Method *RClass::findMethod(const MethodCall &call) const {
  const char * methodName = call.method()->name();
  SEXP fun = findVarInFrame(env(), install(methodName));
  Method *meth = NULL;
  if (fun != R_UnboundValue && TYPEOF(fun) == CLOSXP)
    meth = new RMethod(this, methodName, fun);
  else meth = parent()->findMethod(call);
  return meth;
}

bool
RClass::hasMethod(const char *name, Method::Qualifiers qualifiers) const {
  bool found = parent()->hasMethod(name, qualifiers | Method::NotPrivate);
  if (!found) {
    SEXP fun = findVarInFrame(env(), install(name));
    if (fun != R_UnboundValue && TYPEOF(fun) == CLOSXP)
      found = (RMethod(this, name, fun).qualifiers() & qualifiers) ==
        qualifiers;
  }
  return found;
}

QHash<const char *, int> RClass::enumValues() const {
  return smokeBase()->enumValues();
}
