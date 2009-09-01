#include "SmokeObject.hpp"
#include "SmokeClass.hpp"
#include "RQtModule.hpp"

#include "wrap.hpp"


/* One SmokeObject for each object,
   to ensure 1-1 mapping from Qt objects to R objects */
QHash<void *, SmokeObject *> SmokeObject::instances;

SmokeObject * SmokeObject::fromPtr(void *ptr, Class *klass,
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
SmokeObject::externalPtrFromPtr(void *ptr, Class *klass,
                                bool allocated, bool copy)
{
  return fromPtr(ptr, klass, allocated, copy)->externalPtr();
}

SEXP
SmokeObject::externalPtrFromPtr(void *ptr, Smoke *smoke, const char *name,
                                bool allocated, bool copy)
{
  return externalPtrFromPtr(ptr, Class::fromSmokeName(smoke, name), allocated,
                            copy);
}

SmokeObject * SmokeObject::fromExternalPtr(SEXP externalPtr)
{
  if (externalPtr == R_NilValue)
    return NULL;
  return unwrapPointer(externalPtr, SmokeObject);
}

/* hash SmokeObject to SEXP externalptr, no reference counting */
QHash<const SmokeObject *, SEXP> SmokeObject::externalPtrs;

SmokeObject::SmokeObject(void *ptr, Class *klass, bool allocated)
  : _ptr(ptr), _klass(klass), _allocated(allocated)
{
  _klass = Class::fromSmokeId(smoke(), module()->resolveClassId(this));
}

static void finalizeSmokeObject(SEXP obj) {
  SmokeObject *so = SmokeObject::fromExternalPtr(obj);
  delete so;
}

SEXP SmokeObject::createExternalPtr() const {
  QList<QByteArray> classes;
  Class *c = _klass;
  classes.append(c->name());
  foreach(c, c->ancestors()) {
    classes.append(c->name());
  }
  return wrapPointer(_ptr, classes, finalizeSmokeObject);
}

SEXP SmokeObject::externalPtr() const {  
  SEXP value;
  value = externalPtrs[this];
  if (!value) {
    value = createExternalPtr();
    externalPtrs[this] = value;
  }
  return value;
}

Smoke *SmokeObject::smoke() const { return _klass->smokeBase()->smoke(); }
int SmokeObject::classId() const { return _klass->smokeBase()->classId(); }

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
  instances.remove(_ptr);
  // FIXME: set extptr class to invalid?
  externalPtrs.remove(this);
  if (_allocated && !memoryIsOwned()) {
    const char *cname = klass()->name();
    char *destructor = new char[strlen(cname) + 2];
    destructor[0] = '~';
    strcpy(destructor + 1, cname);
    invokeMethod(destructor);
    delete[] destructor;
  }
}
