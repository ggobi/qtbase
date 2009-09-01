#ifndef CLASS_H
#define CLASS_H

#include <QHash>

class Method;
class MethodCall;
class SmokeClass;
class Smoke;
class ClassFactory;

typedef struct SEXPREC* SEXP;

class Class {
public:

  /* Virtual interface */
  
  virtual const char* name() const = 0;
  virtual Method *findMethod(const MethodCall &call) const = 0;
  virtual QList<Method *> methods() const = 0;
  virtual QList<Class *> ancestors() const = 0;
  virtual const SmokeClass *smokeBase() const = 0;
  
  
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
  
  static Class* fromSmokeId(Smoke *smoke, int classId);
  static Class* fromSmokeName(Smoke *smoke, const char *name);
  static Class* fromSexp(SEXP sexp);
  
  static ClassFactory *classFactory() { return _classFactory; }
  static void setClassFactory(ClassFactory *factory) {
    if (factory) _classFactory = factory;
  }

private:
  static ClassFactory *_classFactory;
  static QHash<const char *, Class *> _classMap;

};

#endif
