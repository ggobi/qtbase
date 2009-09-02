#ifndef R_CLASS_H
#define R_CLASS_H

#include "Class.hpp"

typedef struct SEXPREC* SEXP;

/* Wrapper around the SEXP (function?) representing an R class */

class RClass : public Class {
public:
  RClass() : _klass(NULL) { }
  RClass(SEXP klass) : _klass(klass) { }

  /* Class implementation */
  virtual const char* name() const;
  virtual Method *findMethod(const MethodCall &call);
  virtual const SmokeClass *smokeBase() const;
  virtual QList<Method *> methods() const;
  virtual QList<const Class *> parents() const;
  
  /* R specific accessors */
  SEXP env() const;
  inline SEXP sexp() const { return _klass; }
  inline bool isNull() const { return _klass == NULL; }
  
private:

  const Class* parent() const; // support only single inheritance
  
  SEXP _klass;
};

#endif
