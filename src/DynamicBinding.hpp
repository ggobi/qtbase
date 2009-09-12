#ifndef DYNAMIC_BINDING_H
#define DYNAMIC_BINDING_H

#include "Method.hpp"
#include "SmokeType.hpp"

class MethodSelector;
class MethodCall;

/*
  DynamicBindings enable method calls between runtimes. The binding is
  resolved by the Class.
*/

class DynamicBinding : public virtual Method {
public:
  /* Call an object method */
  DynamicBinding(const char *methodName)
    : _methodName(methodName), _flags(None) { }
  /* Call a static method */
  DynamicBinding(const Class *klass, const char *methodName)
    : _klass(klass), _methodName(methodName), _flags(Static) { }
  /* Obtain parameters from an existing Method */
  DynamicBinding(const Method &method)
    : _klass(method.klass()), _methodName(method.name()),
      _types(method.types()), _flags(method.qualifiers()) { }
  
  virtual void invoke(SmokeObject *obj, Smoke::Stack stack);  
  virtual SEXP invoke(SEXP obj, SEXP args);
  
  virtual const char *name() const { return _methodName; }
  virtual const Class *klass() const { return _klass; }
  virtual QVector<SmokeType> types() const { return _types; }
  virtual Qualifiers qualifiers() const {
    return _flags;
  }
  
private:

  Method::ErrorType methodNotFound(const MethodCall &call);
  
  const Class *_klass;
  const char *_methodName;
  QVector<SmokeType> _types;
  Qualifiers _flags;
};

#endif
