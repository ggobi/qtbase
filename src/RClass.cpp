#include "RClass.hpp"
#include "RMethod.hpp"
#include "RProperty.hpp"
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
  if (fun != R_UnboundValue && TYPEOF(fun) == CLOSXP &&
      (!call.super() ||
       (RMethod(this, methodName, fun).qualifiers() & Method::Private) == 0))
    meth = new RMethod(this, methodName, fun, call.types());
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

SEXP RClass::metadata() const {
  static SEXP metadataSym = install("metadata");
  return getAttrib(_klass, metadataSym);
}

SEXP RClass::properties() const {
  static SEXP propSym = install("properties");
  return findVarInFrame(metadata(), propSym);
}

enum {
  R_PROP_NAME,
  R_PROP_TYPE,
  R_PROP_READER,
  R_PROP_WRITER
};

Property *RClass::property(const char *name) const {
  Property *prop;
  SEXP rprop = findVarInFrame(properties(), install(name));
  if (rprop != R_UnboundValue) {
    SmokeType type(smokeBase()->smoke(),
                   CHAR(asChar(VECTOR_ELT(rprop, R_PROP_TYPE))));
    prop = new RProperty(name, type, VECTOR_ELT(rprop, R_PROP_READER),
                         VECTOR_ELT(rprop, R_PROP_WRITER));
  }
  else prop = parent()->property(name);
  return prop;
}

bool RClass::implementsMethod(const char *name) const {
  SEXP fun = findVarInFrame(env(), install(name));
  return fun != R_UnboundValue && TYPEOF(fun) == CLOSXP;
}
