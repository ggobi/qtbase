#ifndef METHOD_H
#define METHOD_H

#include <QVector>
#include <QFlags>

#include <smoke.h>

class SmokeObject;
class SmokeType;

typedef struct SEXPREC* SEXP;

/* The method sits at the core of the bindings. Our main purpose is to
   call methods across runtimes. Each method has a name, a class, and
   a list of types (return and parameter).

   There are two ways to invoke a method: from Smoke/C++ or R. This is
   formalized by having the two overloads on invoke().  If a method is
   invoked from the foreign side (e.g. an R function is invoked from
   Smoke), marshalling is required.
   
   Method also supports querying for errors after invocation. For
   example, a dynamic binding might not find the indicated method, or an
   R method might throw an error.
*/

class Class;

class Method {
public:
  enum ErrorType { NoError, InvocationFailed, ImplementationFailed,
                   ImplementationMissing };
  enum Qualifier { None, Static = 1 << 0, Protected = 1 << 1
                   /*Virtual = 1 << 2*/ };
  Q_DECLARE_FLAGS(Qualifiers, Qualifier)

  virtual ~Method() { };
  
  virtual const char *name() const = 0;
  virtual QVector<SmokeType> types() const = 0;
  virtual const Class* klass() const = 0;
  virtual Qualifiers qualifiers() const = 0;
  
  virtual void invoke(SmokeObject *obj, Smoke::Stack stack) = 0;  
  virtual SEXP invoke(SEXP obj, SEXP args) = 0;

  ErrorType lastError() const { return _error; }

protected: 
  inline void setLastError(ErrorType type) {
    _error = type;
  }

private:
  ErrorType _error;
  
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Method::Qualifiers)

#endif
