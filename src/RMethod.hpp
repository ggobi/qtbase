#ifndef R_METHOD_H
#define R_METHOD_H


#include <QVector>

#include "Method.hpp"

class SmokeType;
class MethodCall;

/* A Method bound to an R closure */
class RMethod : public Method {
public:
  RMethod(SEXP closure, Class *klass, const char *name, 
          QVector<SmokeType> types = QVector<SmokeType>(),
          Qualifiers flags = 0)
    : _closure(closure),  _klass(klass), _name(name), _types(types),
      _flags(flags)
  { }

  virtual Class * klass() const { return _klass; }
  virtual const char* name() const { return _name; }
  /* R is dynamic, but the method can be typed if associated with a
     statically typed method (e.g. virtual callback or slot). */
  virtual QVector<SmokeType> types() const { return _types; }
  virtual Qualifiers qualifiers() const { return _flags; }
  
  virtual SEXP invoke(SEXP self, SEXP args);
  virtual void invoke(SmokeObject *obj, Smoke::Stack stack);
  
  SEXP closure() const { return _closure; }
  
private:
  SEXP _closure;
  Class *_klass;
  const char* _name;
  QVector<SmokeType> _types;
  Qualifiers _flags;
};

#endif
