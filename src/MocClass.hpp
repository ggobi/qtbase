#ifndef MOC_CLASS_H
#define MOC_CLASS_H

/* Decorator that looks for methods in a QMetaObject. Delegates all
   other behaviors. If the delegate is not a QObject, the search is
   pointless, but nothing will really break. */

#include "Class.hpp"

class MocClass : public Class {
public:
  MocClass(Class *delegate) : _delegate(delegate), _meta(findMetaObject())
  {
  }
  
  virtual const char* name() const { return _delegate->name(); }
  virtual const SmokeClass *smokeBase() const {
    return _delegate->smokeBase();
  }
  virtual QList<const Class *> parents() const {
    return _delegate->parents();
  }
  
  virtual QList<Method *> methods(Method::Qualifiers qualifiers = Method::None)
    const;
  virtual bool hasMethod(const char *name,
                         Method::Qualifiers qualifiers = Method::None) const;
  virtual Method *findMethod(const MethodCall &call) const;

  const QMetaObject *metaObject() const { return _meta; }
  
private:
  int findMethodId(Smoke *smoke, const QMetaObject *meta, const char *name,
                   SEXP args) const;
  const QMetaObject *findMetaObject() const;

  Class *_delegate;
  const QMetaObject *_meta;
  mutable QHash<QByteArray, int> _methods; // map from name (not signature)
};

#endif
  
