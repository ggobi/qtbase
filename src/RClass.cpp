#include "RClass.hpp"

#include <Rinternals.h>

const char *RClass::name() const {
  static SEXP nameSym = install("name");
  return CHAR(asChar(getAttrib(_klass, nameSym)));
}

SEXP RClass::env() const {
  static SEXP envSym = install("env");
  return getAttrib(_klass, envSym);
}

