#include <QSet>

#include "InstanceObjectTable.hpp"
#include "SmokeObject.hpp"
#include "Class.hpp"
#include "wrap.hpp"

SmokeObject *InstanceObjectTable::instanceFromSexp(SEXP sexp) {
  checkPointer(sexp, InstanceObjectTable);
  ObjectTable *table = ObjectTable::fromSexp(sexp);
  return static_cast<InstanceObjectTable *>(table)->instance();
}

QList<QByteArray> InstanceObjectTable::sexpClasses() const {
  QList<QByteArray> classes = ObjectTable::sexpClasses();
  classes.prepend("InstanceObjectTable");
  return classes;
}

SEXP InstanceObjectTable::methodClosure(const char *name) {
  static SEXP qtbaseNS = R_FindNamespace(mkString("qtbase"));
  static SEXP qinvokeSym = install("qinvoke");
  SEXP f, pf, body;
  PROTECT(f = allocSExp(CLOSXP));
  SET_CLOENV(f, qtbaseNS);
  pf = allocList(1);
  SET_FORMALS(f, pf);
  SET_TAG(pf, R_DotsSymbol);
  SETCAR(pf, R_MissingArg);
  PROTECT(body =
          lang4(qinvokeSym, _instance->sexp(), mkString(name), R_DotsSymbol));
  SET_BODY(f, body);
  UNPROTECT(2);
  return f;
}

Rboolean
InstanceObjectTable::exists(const char * name, Rboolean *canCache) {
  bool found = FALSE;
  checkInstance();
  if (canCache) *canCache = TRUE;
  if (_internal)
    found = !qstrcmp(name, "this") ||
      findVarInFrame(fieldEnv(), install(name)) != R_UnboundValue ||
      enumValue(name) != R_UnboundValue;
  if (!found) {
    Method::Qualifiers qual = Method::None;
    if (!_internal)
      qual |= Method::Public | Method::NotStatic;
    found = _instance->klass()->hasMethod(name, qual);
  }
  return (Rboolean)found;
}

SEXP InstanceObjectTable::enumValue(const char *name) {
  SEXP ans = R_UnboundValue;
  int val = _instance->klass()->enumValues().value(name, NA_INTEGER);
  if (val != NA_INTEGER) {
    PROTECT(ans = ScalarInteger(val));
    setAttrib(ans, R_ClassSymbol, mkString("QtEnum"));
    UNPROTECT(1);
  }
  return ans;
}
   
SEXP InstanceObjectTable::get(const char * name, Rboolean* canCache) {
  SEXP ans = R_UnboundValue;
  checkInstance();
  if (canCache) *canCache = TRUE;
  if (_internal) {
    if (!qstrcmp(name, "this"))
      ans = _instance->internalSexp(R_EmptyEnv);
    else ans = findVarInFrame(fieldEnv(), install(name));
    if (ans == R_UnboundValue)
      ans = enumValue(name);
  }
  if (ans == R_UnboundValue && exists(name, canCache))
    ans = methodClosure(name); // make a wrapper for method
  return ans;
}

int InstanceObjectTable::remove(const char * name) {
  checkInstance();
  Q_UNUSED(name);
  return 0;
}

SEXP InstanceObjectTable::assign(const char * name, SEXP value) {
  checkInstance();
  if (_internal) {
    SEXP sym = install(name);
    defineVar(sym, value, fieldEnv());
    return sym;
  }
  return R_NilValue;
}

SEXP InstanceObjectTable::objects() {
  Method::Qualifiers qual = Method::NotStatic;
  QList<Method *> methods; 
  SEXP nameVector;
  QSet<const char *> nameSet;

  checkInstance();

  if (!_internal)
    qual |= Method::Public;
  methods = _instance->klass()->methods(qual);
  
  foreach(Method *m, methods) // put names into set to get unique ones
    nameSet.insert(m->name());

  int numFields = 0;
  SEXP fields;
  if (_internal) {
    PROTECT(fields = R_lsInternal(fieldEnv(), TRUE));
    numFields = length(fields);
  }
  
  PROTECT(nameVector = allocVector(STRSXP, nameSet.size() + numFields));
  int i = 0; // iterate over set, populate R vector
  foreach(const char *name, nameSet)
    SET_STRING_ELT(nameVector, i++, mkChar(name));
  for(int j = 0; i < length(nameVector); i++, j++)
    SET_STRING_ELT(nameVector, i, STRING_ELT(fields, j));

  while(!methods.isEmpty())
    delete methods.takeFirst();  

  UNPROTECT(1);
  if (_internal)
    UNPROTECT(1);
  
  return nameVector;
}

SEXP InstanceObjectTable::fieldEnv() {
  return _instance->fieldEnv();
}

void InstanceObjectTable::checkInstance() {
  if (!_instance)
    error("Attempt to access invalid instance");
}

// We (R) have taken ownership of the instance
InstanceObjectTable::~InstanceObjectTable() {
  if (_instance) { // might have been orphaned
    if (_internal)
      _instance->invalidateInternalTable();
    else _instance->invalidateSexp();
  }
}
