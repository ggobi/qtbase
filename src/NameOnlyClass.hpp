#ifndef NAME_ONLY_CLASS_H
#define NAME_ONLY_CLASS_H

/* A class that extends another class but does not provide any members
   (at least as far as we are concerned). */

#include "Class.hpp"

class NameOnlyClass : public Class {
public:
  NameOnlyClass(const char *name, const Class *parent)
      : _parent(parent), _name(name)
  {
  }
  
  virtual const char* name() const { return _name; }

  virtual const SmokeClass *smokeBase() const {
    return _parent->smokeBase();
  }

  virtual QList<const Class *> parents() const {
    QList<const Class *> p;
    p += _parent;
    return p;
  }
  
  virtual QList<Method *> methods(Method::Qualifiers qualifiers = Method::None)
    const
  {
    return _parent->methods(qualifiers);
  }
  virtual bool hasMethod(const char *name,
                         Method::Qualifiers qualifiers = Method::None) const
  {
    return _parent->hasMethod(name, qualifiers);
  }
  virtual Method *findMethod(const MethodCall &call) const {
    return _parent->findMethod(call);
  }
  virtual bool implementsMethod(const char *name) const {
    return _parent->implementsMethod(name);
  }
  virtual QHash<const char *, int> enumValues() const {
    return _parent->enumValues();
  }
  virtual Property *property(const char *name) const {
    return _parent->property(name);
  }

private:
  const Class *_parent;
  const char *_name;
};

#endif
  
