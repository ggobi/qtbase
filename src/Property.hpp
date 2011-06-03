#ifndef PROPERTY_H
#define PROPERTY_H

#include <smoke.h>

class SmokeObject;

typedef struct SEXPREC* SEXP;

class Property {
public:

  virtual ~Property() { }
  
  virtual const char *name() const = 0;
  
  virtual SEXP read(SEXP obj) const = 0;
  virtual Smoke::StackItem read(SmokeObject *so) const = 0;

  virtual bool write(SEXP obj, SEXP val) = 0;
  virtual bool write(SmokeObject *so, const Smoke::StackItem &item) = 0;

  virtual bool isReadable() const = 0;
  virtual bool isWritable() const = 0;
};

#endif
