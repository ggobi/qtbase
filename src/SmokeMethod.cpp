#include "SmokeMethod.hpp"
#include "SmokeObject.hpp"
#include "RQtModule.hpp"

void SmokeMethod::invoke(SmokeObject *obj, Smoke::Stack stack) {
  if (!obj && !isStatic() && !isConstructor()) {
    qCritical("'obj' is NULL but %s is not a class method\n", name());
    setLastError(InvocationFailed);
    return;
  }
    
  Smoke::Class cl = _smoke->classes[classId()];
  Smoke::ClassFn fn = cl.classFn;
  void * ptr = 0;
    
  if (obj != 0) {
    ptr = obj->castPtr(cl.className);
  }
    
  (*fn)(_m->method, ptr, stack);
  
  if (isConstructor()) {
    Smoke::StackItem s[2];
    s[1].s_voidp = RQtModule::module(_smoke)->binding();
    (*fn)(0, stack[0].s_voidp, s);
  }

  setLastError(NoError);
}

QVector<SmokeType> SmokeMethod::types() const {
  Smoke::Index *argTypes = args();
  QVector<SmokeType> t(_m->numArgs + 1);
  t[0] = returnType();
  for (int i = 0; i < _m->numArgs; i++)
    t[i+1] = SmokeType(_smoke, argTypes[i]);
  return t;
}

const Class *SmokeMethod::klass() const {
  return Class::fromSmokeId(_smoke, classId());
}
