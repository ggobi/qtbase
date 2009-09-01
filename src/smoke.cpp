#include <smoke.h>

#include "Method.hpp"
#include "Class.hpp"
#include "SmokeList.hpp"
#include "SmokeType.hpp"
#include "RQtModule.hpp"

#include "wrap.hpp"

/* Access Smoke Metadata from R */

Smoke *asSmoke(SEXP rsmoke) {
  return unwrapPointer(rsmoke, Smoke);
}

SEXP asRSmoke(Smoke *smoke) {
  QList<QByteArray> classes;
  classes.append("Smoke");
  return wrapPointer(smoke, classes);
}

enum {
  MD_NAME,
  MD_RETURN,
  MD_ARGS,
  MD_PROTECTED,
  MD_STATIC,
  MD_LAST
};

SEXP asRMethodDesc(Method *method) {
  SEXP methodDesc;
  PROTECT(methodDesc = allocVector(VECSXP, MD_LAST));
  SET_VECTOR_ELT(methodDesc, MD_NAME,
                 mkString(method->name()));
  QVector<SmokeType> types = method->types();
  SET_VECTOR_ELT(methodDesc, MD_RETURN, mkString(types[0].name()));
  SEXP args = allocVector(STRSXP, types.size()-1);
  SET_VECTOR_ELT(methodDesc, MD_ARGS, args);
  for (int i = 0; i < length(args); i++)
    SET_STRING_ELT(args, i, mkChar(types[i+1].name()));
  SET_VECTOR_ELT(methodDesc, MD_PROTECTED,
                 ScalarLogical(method->qualifiers() & Method::Protected));
  SET_VECTOR_ELT(methodDesc, MD_STATIC,
                 ScalarLogical(method->qualifiers() & Method::Static));
  setAttrib(methodDesc, R_ClassSymbol, mkString("QtMethodDesc"));
  UNPROTECT(1);
  return methodDesc;
}

/* Get a list of method names with their argument types */
SEXP
qt_qmethods(SEXP klass)
{
  SEXP result, resultNames;
  Class *c = Class::fromSexp(klass);
  QList<Method *> methods = c->methods();
  int i = 0;
  PROTECT(result = allocVector(VECSXP, methods.size()));
  resultNames = allocVector(STRSXP, length(result));
  setAttrib(result, R_NamesSymbol, resultNames);
  while(!methods.isEmpty()) {
    Method * m = methods.takeFirst();
    SET_STRING_ELT(resultNames, i, mkChar(m->name()));
    SET_VECTOR_ELT(result, i++, asRMethodDesc(m));
    delete m;
  }
  UNPROTECT(1);
  return result;
}

SEXP
qt_qclasses(SEXP rsmoke) {
  Smoke *smoke = asSmoke(rsmoke);
  SEXP rclasses;
  PROTECT(rclasses = allocVector(STRSXP, smoke->numClasses));
  for (int i = 0; i < length(rclasses); i++)
    SET_STRING_ELT(rclasses, i, mkChar(smoke->classes[i].className));
  UNPROTECT(1);
  return rclasses;
}

SEXP
qt_qsmokes(void) {
  SmokeList smokes = RQtModule::smokes();
  SEXP rsmokes, rnames;
  PROTECT(rsmokes = allocVector(VECSXP, smokes.size()));
  rnames = allocVector(STRSXP, length(rsmokes));
  setAttrib(rsmokes, R_NamesSymbol, rnames);
  for (int i = 0; i < length(rsmokes); i++) {
    Smoke *smoke = smokes[i];
    SET_VECTOR_ELT(rsmokes, i, asRSmoke(smoke));
    SET_STRING_ELT(rnames, i, mkChar(smoke->moduleName()));
  }
  UNPROTECT(1);
  return rsmokes;
}
