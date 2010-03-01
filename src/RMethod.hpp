#ifndef R_METHOD_H
#define R_METHOD_H


#include <QVector>

#include "Method.hpp"

class SmokeType;
class MethodCall;

/* A Method bound to an R closure */
class RMethod : public Method {
public:
  RMethod(const Class *klass, const char *name, SEXP closure,
          QVector<SmokeType> types = QVector<SmokeType>(),
          SEXP userData = NULL)
    : _closure(closure),  _klass(klass), _name(name), _types(types),
      _userData(userData)
  { }

  virtual const Class * klass() const { return _klass; }
  virtual const char* name() const { return _name; }
  /* R is dynamic, but the method can be typed if associated with a
     statically typed method (e.g. virtual callback or slot). */
  virtual QVector<SmokeType> types() const { return _types; }
  virtual Qualifiers qualifiers() const;
  
  virtual SEXP invoke(SEXP self, SEXP args);
  virtual void invoke(SmokeObject *obj, Smoke::Stack stack);
  
  SEXP closure() const { return _closure; }
  SEXP userData() const { return _userData; }
  
private:
  SEXP _closure;
  const Class *_klass;
  const char* _name;
  QVector<SmokeType> _types;
  Qualifiers _flags;
  SEXP _userData;
};

#endif
