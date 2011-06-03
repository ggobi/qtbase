#ifndef MOC_CLASS_H
#define MOC_CLASS_H

/* Decorator that looks for methods, properties and the class name in
   a QMetaObject. Delegates all other behaviors. If the delegate does
   not represent a QObject, the search is pointless, but nothing will
   really break. */

// FIXME: metaObject() is a virtual, so subclasses can actually
// override it to provide per-instance methods. This does not fit into
// the conventional paradigm, so we may need to provide a custom
// InstanceObjectTable that performs a instance-specific
// query. Otherwise, this class is pretty useless.

#include "Class.hpp"

class MocClass : public Class {
public:
  MocClass(const Class *delegate) : _delegate(delegate), _meta(findMetaObject())
  {
  }
  MocClass(const Class *delegate, const QMetaObject *meta)
    : _delegate(delegate), _meta(meta)
  {
  }
  
  virtual const char* name() const;
  virtual const SmokeClass *smokeBase() const {
    return _delegate->smokeBase();
  }
  // cannot come from QMetaObject, which is limited to single inheritance
  virtual QList<const Class *> parents() const {
    return _delegate->parents();
  }
  
  virtual QList<Method *> methods(Method::Qualifiers qualifiers = Method::None)
    const;
  virtual bool hasMethod(const char *name,
                         Method::Qualifiers qualifiers = Method::None) const;
  virtual Method *findMethod(const MethodCall &call) const;
  virtual bool implementsMethod(const char *name) const;
  virtual QHash<const char *, int> enumValues() const;
  virtual Property *property(const char *name) const;
  
  const QMetaObject *metaObject() const { return _meta; }
  
private:
  int findMethodId(Smoke *smoke, const QMetaObject *meta, const char *name,
                   SEXP args) const;
  const QMetaObject *findMetaObject() const;

  const Class *_delegate;
  const QMetaObject *_meta;
  mutable QHash<QByteArray, int> _methods; // map from name (not signature)
};

#endif
  
