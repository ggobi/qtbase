#include <QByteArray>
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
  MD_SIG,
  MD_PROTECTED,
  MD_STATIC,
  MD_LAST
};

static const char* rTypeName(const SmokeType &type) {
  QByteArray name(type.name());
  name.replace("const ", "");
  name.replace("&", "");
  return name.constData();
}

/* Get a list of method names with their argument types */
extern "C"
SEXP qt_qmethods(SEXP klass)
{
  SEXP result;
  const Class *c = Class::fromSexp(klass);
  QList<Method *> methods = c->methods();
  int i = 0;
  
  PROTECT(result = allocVector(VECSXP, MD_LAST));
  SEXP resultName = allocVector(STRSXP, methods.size());
  SET_VECTOR_ELT(result, MD_NAME, resultName);
  SEXP resultReturn = allocVector(STRSXP, methods.size());
  SET_VECTOR_ELT(result, MD_RETURN, resultReturn);
  SEXP resultSig = allocVector(STRSXP, methods.size());
  SET_VECTOR_ELT(result, MD_SIG, resultSig);
  SEXP resultProtected = allocVector(LGLSXP, methods.size());
  SET_VECTOR_ELT(result, MD_PROTECTED, resultProtected);
  SEXP resultStatic = allocVector(LGLSXP, methods.size());
  SET_VECTOR_ELT(result, MD_STATIC, resultStatic);
  
  while(!methods.isEmpty()) {
    Method * m = methods.takeFirst();
    QVector<SmokeType> types = m->types();
    SET_STRING_ELT(resultName, i, mkChar(m->name()));
    SEXP ret = types[0].isVoid() ? mkChar(rTypeName(types[0])) : R_BlankString;
    SET_STRING_ELT(resultReturn, i, ret);
    QByteArray sig = m->name();
    sig += "(";
    for (int j = 1; j < types.size(); j++) {
      sig += rTypeName(types[j]);
      if (j+1 < types.size())
        sig += ", ";
    }
    sig += ")";
    SET_STRING_ELT(resultSig, i, mkChar(sig.constData()));
    LOGICAL(resultProtected)[i] = m->qualifiers() & Method::Protected;
    LOGICAL(resultStatic)[i] = m->qualifiers() & Method::Static;
    delete m;
    i++;
  }
  
  UNPROTECT(1);
  return result;
}

extern "C"
SEXP qt_qclasses(SEXP rsmoke) {
  Smoke *smoke = asSmoke(rsmoke);
  SEXP rclasses;
  PROTECT(rclasses = allocVector(STRSXP, smoke->numClasses));
  for (int i = 0; i < length(rclasses); i++) {
    SET_STRING_ELT(rclasses, i, mkChar(smoke->classes[i+1].className));
  }
  UNPROTECT(1);
  return rclasses;
}

extern "C"
SEXP qt_qsmokes(void) {
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