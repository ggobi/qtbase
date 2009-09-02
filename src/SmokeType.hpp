#ifndef SMOKE_TYPE_H
#define SMOKE_TYPE_H

#include <QByteArray> // for qstrcmp

#include <smoke.h>

/* Represents a type in Smoke, i.e. a C++ type. It should not be
   necessary to represent R types, as R is dynamically typed. */
class SmokeType {
public:
  SmokeType() : _t(0), _smoke(0), _id(0) {}
  SmokeType(Smoke *s, Smoke::Index i) : _smoke(s), _id(i) {
    findType();
  }
  SmokeType(Smoke::ModuleIndex ind) : _smoke(ind.smoke), _id(ind.index) {
    findType();
  }
  
  // accessors
  inline Smoke *smoke() const { return _smoke; }
  inline Smoke::Index typeId() const { return _id; }
  inline const Smoke::Type &type() const { return *_t; }
  inline unsigned short flags() const { return _t->flags; }
  inline unsigned short elem() const { return _t->flags & Smoke::tf_elem; }
  inline const char *name() const { return _t->name; }
  inline Smoke::Index classId() const { return _t->classId; }
  inline bool isVoid() const { return _id == 0; }

  // tests
  inline bool isStack() const {
    return ((flags() & Smoke::tf_ref) == Smoke::tf_stack);
  }
  inline bool isPtr() const {
    return ((flags() & Smoke::tf_ref) == Smoke::tf_ptr);
  }
  inline bool isRef() const {
    return ((flags() & Smoke::tf_ref) == Smoke::tf_ref);
  }
  inline bool isConst() const { return (flags() & Smoke::tf_const); }
  inline bool isClass() const {
    if(elem() == Smoke::t_class)
      return classId() ? true : false;
    return false;
  }
  
  bool operator ==(const SmokeType &b) const {
    const SmokeType &a = *this;
    if(a.name() == b.name()) return true;
    if(a.name() && b.name() && qstrcmp(a.name(), b.name()) == 0)
      return true;
    return false;
  }
  bool operator !=(const SmokeType &b) const {
    const SmokeType &a = *this;
    return !(a == b);
  }
  
private:
  void findType() {
    if(_id < 0 || _id > _smoke->numTypes) _id = 0;
    _t = _smoke->types + _id;
  }

  Smoke::Type *_t;		// derived from _smoke and _id, but cached
  Smoke *_smoke;
  Smoke::Index _id;
};

#endif
