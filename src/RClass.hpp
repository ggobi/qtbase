#ifndef R_CLASS_H
#define R_CLASS_H

#include "Class.hpp"

typedef struct SEXPREC* SEXP;

/* Wrapper around the SEXP (function?) representing an R class */

class RClass : public Class {
public:
  RClass() : _klass(NULL) { }
  explicit RClass(SEXP klass);

  virtual ~RClass();

  /* Class implementation */
  virtual const char* name() const;
  virtual Method *findMethod(const MethodCall &call) const;
  virtual const SmokeClass *smokeBase() const;
  virtual QList<const Class *> parents() const;
  virtual bool hasMethod(const char *name,
                         Method::Qualifiers qualifiers = Method::None) const;
  virtual QList<Method *> methods(Method::Qualifiers qualifiers = Method::None)
    const;
  virtual bool implementsMethod(const char *name) const;
  virtual QHash<const char *, int> enumValues() const;
  virtual Property *property(const char *name) const;
    
  /* R specific accessors */
  inline SEXP sexp() const { return _klass; }
  inline bool isNull() const { return _klass == NULL; }

private:

  const Class* parent() const; // support only single inheritance
  SEXP metadata() const;
  SEXP properties() const;
  SEXP methodEnv() const;
  
  SEXP _klass;
};

#endif
