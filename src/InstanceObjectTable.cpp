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
  if (canCache) *canCache = TRUE;
  return (Rboolean)
    _instance->klass()->hasMethod(name, Method::NotStatic | Method::Public);
}
SEXP InstanceObjectTable::get(const char * name, Rboolean* canCache) {
  if (canCache) *canCache = TRUE;
  SEXP ans = R_UnboundValue;
  if (exists(name, canCache)) // we have a method, make a wrapper
    ans = methodClosure(name);
  return ans;
}
int InstanceObjectTable::remove(const char * name) {
  Q_UNUSED(name);
  return 0;
}
SEXP InstanceObjectTable::assign(const char * name, SEXP value) {
  Q_UNUSED(name);
  Q_UNUSED(value);
  return R_NilValue;
}
SEXP InstanceObjectTable::objects() {
  QList<Method *> methods =
    _instance->klass()->methods(Method::NotStatic | Method::Public);
  SEXP nameVector;
  QSet<const char *> nameSet;

  foreach(Method *m, methods) // put names into set to get unique ones
    nameSet.insert(m->name());
  
  PROTECT(nameVector = allocVector(STRSXP, nameSet.size()));
  int i = 0; // iterate over set, populate R vector
  foreach(const char *name, nameSet)
    SET_STRING_ELT(nameVector, i++, mkChar(name));
  UNPROTECT(1);
  
  while(!methods.isEmpty())
    delete methods.takeFirst();  
  
  return nameVector;
}

InstanceObjectTable::~InstanceObjectTable() {
  SmokeObject *so = _instance;
  so->invalidateSexp();
  if (so->allocated() && !so->memoryIsOwned()) {
    //qDebug("Destructing referant");
    const char *cname = so->className();
    char *destructor = new char[strlen(cname) + 2];
    destructor[0] = '~';
    strcpy(destructor + 1, cname);
    so->invokeMethod(destructor); // causes 'so' to be destructed
    delete[] destructor;
  }
}
