#ifndef CLASS_H
#define CLASS_H

#include <QHash>

#include "Method.hpp" // for Method::Qualifiers

class MethodCall;
class SmokeClass;
class Smoke;
class ClassFactory;
class InstanceObjectTable;
class Property;

typedef struct SEXPREC* SEXP;

class Class {
public:

  /* Virtual interface */

  virtual ~Class() { }
  
  virtual const char* name() const = 0;
  
  virtual Method *findMethod(const MethodCall &call) const = 0;
  virtual QList<Method *> methods(Method::Qualifiers qualifiers = Method::None)
    const = 0;
  virtual bool hasMethod(const char *name,
                         Method::Qualifiers qualifiers = Method::None)
    const = 0;
  virtual bool implementsMethod(const char *name) const = 0;
  
  virtual QHash<const char *, int> enumValues() const = 0;

  // or virtual QHash<const char *, Property*> properties() const = 0;
  virtual Property *property(const char *name) const = 0;
  
  virtual QList<const Class *> parents() const = 0;
  virtual const SmokeClass *smokeBase() const = 0;

  /* Factory of instance databases for R environments */
  virtual InstanceObjectTable *createObjectTable(SmokeObject *obj) const;
  
  /* TODO if we support user static methods
  SEXP staticEnv() const;
  virtual SEXP createClassObjectTable() const;
  */
  
  /* Some utilities */
  
  QList<const Class *> ancestors() const;

  // Often want to know this to optimize e.g. callback handlers
  bool userImplementsMethod(const char *methodName) const;
  
  /* Whether the Class objects represent the same class. */
  bool operator ==(const Class &b) const {
    const Class &a = *this;
    const char *aname = a.name();
    const char *bname = b.name();
    if(aname == bname) return true;
    if(aname && bname && qstrcmp(aname, bname) == 0)
      return true;
    return false;
  }
  bool operator !=(const Class &b) const {
    const Class &a = *this;
    return !(a == b);
  }

  /* Global instance management */
  /* We want only one Class instance per class in the runtime. We
     statically store references to the Class instances, which are
     lazily instantiated through this API. For the sake of
     extensibility, we allow the client to override the construction
     of the Class instances through a factory.
  */
  
  static const Class* fromSmokeId(Smoke *smoke, int classId);
  static const Class* fromSmokeName(Smoke *smoke, const char *name);
  static const Class* fromSexp(SEXP sexp);
  
  static ClassFactory *classFactory() { return _classFactory; }
  static void setClassFactory(ClassFactory *factory) {
    if (factory) _classFactory = factory;
  }
  
private:
  static ClassFactory *_classFactory;
  static QHash<const char *, const Class *> _classMap;

};

#endif
