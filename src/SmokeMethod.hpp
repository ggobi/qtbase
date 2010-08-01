#ifndef SMOKE_METHOD_H
#define SMOKE_METHOD_H


#include "ForeignMethod.hpp"
#include "SmokeClass.hpp"

/* High-level wrapper around Smoke::Method */

class SmokeMethod : public ForeignMethod {
private:
  Smoke::Method *_m;
  Smoke *_smoke;
  Smoke::Index _id;

  void findMethod() {
    if(_id < 0 || _id > _smoke->numMethods) _id = 0;
    _m = _smoke->methods + _id;
  }
  
public:
  SmokeMethod() : _m(0), _smoke(0), _id(0) {}
  SmokeMethod(Smoke *s, Smoke::Index i) : _smoke(s), _id(i) {
    findMethod();
  }
  SmokeMethod(Smoke::ModuleIndex ind) : _smoke(ind.smoke), _id(ind.index)
  {
    findMethod();
  }
    
  inline Smoke *smoke() const { return _smoke; }
  inline Smoke::Index methodId() const { return _id; }
  inline const Smoke::Method &method() const { return *_m; }
  virtual Qualifiers qualifiers() const {
    Qualifiers flags = NotPrivate;
    if (_m->flags & Smoke::mf_protected)
      flags |= Protected;
    else flags |= Public;
    if (_m->flags & Smoke::mf_static)
      flags |= Static;
    else flags |= NotStatic;
    if (_m->flags & Smoke::mf_ctor) {
      flags |= Constructor;
      if (!(_m->flags & Smoke::mf_explicit))
        flags |= Implicit;
    }
    return flags;
  }

  virtual inline const char *name() const {
    return _smoke->methodNames[_m->name];
  }
  inline Smoke::Index classId() const { return _m->classId; }
  virtual const Class *klass() const;
  inline Smoke::Index *args() const { return _smoke->argumentList + _m->args; }
  inline Smoke::Index returnIndex() const { return _m->ret; }
  inline SmokeType returnType() const {
    return SmokeType(_smoke, returnIndex());
  }
  virtual QVector<SmokeType> types() const;
  
  inline bool isStatic() const { return _m->flags & Smoke::mf_static; }
  inline bool isConst() const { return _m->flags & Smoke::mf_const; }
  inline bool isConstructor() const { return _m->flags & Smoke::mf_ctor; }
  inline bool isCopyConstructor() const {
    return _m->flags & Smoke::mf_copyctor;
  }
  inline bool isInternal() const { return _m->flags & Smoke::mf_internal; }
  inline bool isDestructor() const { return _m->flags & Smoke::mf_dtor; }
  inline bool isProtected() const { return _m->flags & Smoke::mf_protected; }
  inline bool isVirtual() const { return _m->flags & Smoke::mf_virtual; }
  inline bool isExplicit() const { return _m->flags & Smoke::mf_explicit; }
  
  virtual void invoke(SmokeObject *obj, Smoke::Stack stack);
};

#endif
