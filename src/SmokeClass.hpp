#ifndef SMOKE_CLASS_H
#define SMOKE_CLASS_H

#include "Class.hpp"
#include "SmokeType.hpp"

class SmokeClass : public Class {
public:
  SmokeClass() : _c(NULL), _smoke(NULL), _id(0), enumValuesCached(false) { }
  SmokeClass(const SmokeType &t) : _smoke(t.smoke()), _id(t.classId())  {
    init();
  }
  SmokeClass(Smoke *smoke, Smoke::Index id) : _smoke(smoke), _id(id)
  {
    init();
  }
  SmokeClass(Smoke *smoke, const char *name)
    : _smoke(smoke), _id(smoke->idClass(name).index)
  {
    init();
  }
  SmokeClass(Smoke::ModuleIndex ind) : _smoke(ind.smoke), _id(ind.index)
  {
    init();
  }

  /* Class implementation */
  virtual const char *name() const { return _c->className; }
  virtual const SmokeClass *smokeBase() const { return this; }
  
  virtual QList<Method *> methods(Method::Qualifiers qualifiers = Method::None)
    const;
  virtual bool hasMethod(const char *name,
                         Method::Qualifiers qualifiers = Method::None) const;
  virtual Method *findMethod(const MethodCall &call) const;
  virtual QHash<const char *, int> enumValues() const;
  virtual Property *property(const char *name) const;
  virtual QList<const Class *> parents() const;
  virtual bool implementsMethod(const char *name) const;
  
  inline const Smoke::Class &c() const { return *_c; }
  inline Smoke::Index classId() const { return _id; }
  inline Smoke::ClassFn classFn() const { return _c->classFn; }
  inline Smoke::EnumFn enumFn() const { return _c->enumFn; }
  inline Smoke *smoke() const { return _smoke; }
  
  bool isa(const SmokeClass &sc) const {
    // This is a sick function, if I do say so myself
    if(*this == sc) return true;
    Smoke::Index *parents = _smoke->inheritanceList + _c->parents;
    for(int i = 0; parents[i]; i++) {
      if(SmokeClass(_smoke, parents[i]).isa(sc)) return true;
    }
    return false;
  }

  inline unsigned short flags() const { return _c->flags; }
  inline bool hasConstructor() const { return flags() & Smoke::cf_constructor; }
  inline bool hasCopy() const { return flags() & Smoke::cf_deepcopy; }
  inline bool hasVirtual() const { return flags() & Smoke::cf_virtual; }
  inline bool hasFire() const { return !(flags() & Smoke::cf_undefined); }

private:

  Smoke::ModuleIndex findIndex(const MethodCall& call) const;
  QByteArray mungedMethodName(const MethodCall &call) const;
  QHash<const char *, int> createEnumValuesMap() const;
  void findMethodRange();
  void init() { // common initialization code
    _c = _smoke->classes + _id;
    findMethodRange();
    enumValuesCached = false;
  }
  
  Smoke::Class *_c;
  Smoke *_smoke;
  Smoke::Index _id;
  mutable QHash<QByteArray, Method::Qualifiers> _methodQuals;
  int methmin;
  int methmax;
  mutable QHash<const char *, int> _enumValues;
  mutable bool enumValuesCached;
  mutable QList<const Class *> _parents;
};

#endif
