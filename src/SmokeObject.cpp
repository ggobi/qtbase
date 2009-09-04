#include "SmokeObject.hpp"
#include "SmokeClass.hpp"
#include "RQtModule.hpp"
#include "InstanceObjectTable.hpp"

#include "wrap.hpp"


/* One SmokeObject for each object,
   to ensure 1-1 mapping from Qt objects to R objects */
QHash<void *, SmokeObject *> SmokeObject::instances;

SmokeObject * SmokeObject::fromPtr(void *ptr, const Class *klass,
                                   bool allocated, bool copy)
{
  SmokeObject *so = instances[ptr];
  if (!so) {
    so = new SmokeObject(ptr, klass, allocated);
    if (copy) { // copy the data before storing in hash
      so->_ptr = so->constructCopy();
      so->_allocated = true;
    }
    instances[so->ptr()] = so;
  }
  return so;
}

SmokeObject *SmokeObject::fromPtr(void *ptr, Smoke *smoke, const char *name,
                                  bool allocated, bool copy)
{
  return fromPtr(ptr, Class::fromSmokeName(smoke, name), allocated, copy);
}

SmokeObject *SmokeObject::fromPtr(void *ptr, Smoke *smoke, int classId,
                                  bool allocated, bool copy)
{
  return fromPtr(ptr, Class::fromSmokeId(smoke, classId), allocated, copy);
}

SEXP
SmokeObject::sexpFromPtr(void *ptr, const Class *klass,
                                bool allocated, bool copy)
{
  return fromPtr(ptr, klass, allocated, copy)->sexp();
}

SEXP
SmokeObject::sexpFromPtr(void *ptr, Smoke *smoke, const char *name,
                                bool allocated, bool copy)
{
  return sexpFromPtr(ptr, Class::fromSmokeName(smoke, name), allocated, copy);
}

SmokeObject * SmokeObject::fromSexp(SEXP sexp)
{
  if (!isEnvironment(sexp))
    error("Expected an environment, but got '%s'", type2char(TYPEOF(sexp)));
  return InstanceObjectTable::instanceFromSexp(HASHTAB(sexp));
}

SmokeObject::SmokeObject(void *ptr, const Class *klass, bool allocated)
  : _ptr(ptr), _klass(klass), _allocated(allocated)
{
  _klass = Class::fromSmokeId(smoke(), module()->resolveClassId(this));
}

void SmokeObject::invalidateSexp() {
  _sexp = NULL;
}

SEXP SmokeObject::createSexp() {
  SEXP env, rclasses;
  PROTECT(env = allocSExp(ENVSXP));
  SET_ENCLOS(env, R_EmptyEnv);
  
  QList<const Class *> classes = _klass->ancestors();
  classes.prepend(_klass);
  rclasses = allocVector(STRSXP, classes.size() + 2);
  setAttrib(env, R_ClassSymbol, rclasses);
  for (int i = 0; i < classes.size(); i++)
    SET_STRING_ELT(rclasses, i, mkChar(classes[i]->name()));
  SET_STRING_ELT(rclasses, length(rclasses) - 2, mkChar("UserDefinedDatabase"));
  SET_STRING_ELT(rclasses, length(rclasses) - 1, mkChar("environment"));

  SET_HASHTAB(env, _klass->createObjectTable(this)->sexp());

  UNPROTECT(1);
  return env;
}

SEXP SmokeObject::sexp() {
  if (!_sexp)
    _sexp = createSexp();
  return _sexp;
}

Smoke *SmokeObject::smoke() const { return _klass->smokeBase()->smoke(); }
int SmokeObject::classId() const { return _klass->smokeBase()->classId(); }
const char *SmokeObject::className() const { return _klass->name(); }

RQtModule *SmokeObject::module() const {
  return RQtModule::module(smoke());
}

bool SmokeObject::memoryIsOwned() const {
  return module()->memoryIsOwned(this);
}

// only works for pure Smoke instances, but that may be OK
void * SmokeObject::constructCopy() {
  Smoke *smoke = this->smoke();
  const char *className = _klass->name();
  int classNameLen = strlen(className);

  // copy constructor signature
  QByteArray ccSig(className);
  int pos = ccSig.lastIndexOf("::");
  if (pos != -1) {
    ccSig = ccSig.mid(pos + strlen("::"));
  }
  ccSig.append("#");
  Smoke::ModuleIndex ccId = smoke->findMethodName(className, ccSig);

  char *ccArg = new char[classNameLen + 8];
  sprintf(ccArg, "const %s&", className);

  Smoke::ModuleIndex classIdx = { smoke, classId() };
  Smoke::ModuleIndex ccMeth = smoke->findMethod(classIdx, ccId);

  if (ccMeth.index == 0) {
    qWarning("failed to construct copy: %s %p\n", className, _ptr);
    delete[] ccArg;
    return 0;
  }
  Smoke::Index method = ccMeth.smoke->methodMaps[ccMeth.index].method;
  if (method > 0) {
    delete[] ccArg;
    if (!(ccMeth.smoke->methods[method].flags & Smoke::mf_copyctor)) {
      qCritical("failed to construct copy: %s %p\n", className, _ptr);
      return 0;
    }
    ccMeth.index = method;
  } else {
    // ambiguous method, pick the copy constructor
    Smoke::Index i = -method;
    while (ccMeth.smoke->ambiguousMethodList[i]) {
      if (ccMeth.smoke->methods[ccMeth.smoke->ambiguousMethodList[i]].flags &
          Smoke::mf_copyctor)
        break;
      i++;
    }
    delete[] ccArg;
    ccMeth.index = ccMeth.smoke->ambiguousMethodList[i];
    if (ccMeth.index == 0) {
      qCritical("construct_copy() failed %s %p\n", className, _ptr);
      return 0;
    }
  }

  // Okay, ccMeth is the copy constructor. Time to call it.
  Smoke::StackItem args[2];
  args[0].s_voidp = 0;
  args[1].s_voidp = _ptr;
  Smoke::ClassFn fn = smoke->classes[classId()].classFn;
  (*fn)(smoke->methods[ccMeth.index].method, 0, args);

  // Initialize the binding for the new instance
  Smoke::StackItem s[2];
  s[1].s_voidp = module()->binding();
  (*fn)(0, args[0].s_voidp, s);

  return args[0].s_voidp;
}

void *
SmokeObject::cast(const char *className) const {
  Smoke *smoke = this->smoke();
  return smoke->cast(_ptr, classId(), smoke->idClass(className, true).index);
}

bool // result undefined if class names are not all unique
SmokeObject::instanceOf(const char *className) const {
  Smoke *smoke = this->smoke();
  Smoke::ModuleIndex other = smoke->idClass(className, true);
  return smoke->isDerivedFrom(smoke, classId(), other.smoke, other.index);
}

void
SmokeObject::invokeMethod(const char *name, Smoke::Stack stack) {
  Smoke *smoke = this->smoke();
  Smoke::ModuleIndex nameId = smoke->idMethodName(name);
  Smoke::ModuleIndex classIdx = { smoke, classId() };
  Smoke::ModuleIndex meth = nameId.smoke->findMethod(classIdx, nameId);
  if (meth.index > 0) {
    Smoke::Method &m =
      meth.smoke->methods[meth.smoke->methodMaps[meth.index].method];
    Smoke::ClassFn fn = meth.smoke->classes[m.classId].classFn;
    (*fn)(m.method, _ptr, stack);
  } else {
    error("Cannot find (unambiguous) %s::%s",
          smoke->classes[classIdx.index].className, name);
  }
}

SmokeObject::~SmokeObject() {
  if (_sexp) // sexp still exists, invalidate class
    setAttrib(_sexp, R_ClassSymbol, ScalarString(R_NaString));
  instances.remove(_ptr);
  //  qDebug("Destructing SmokeObject");
}
