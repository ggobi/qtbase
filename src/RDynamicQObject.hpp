#ifndef RDYNAMICQOBJECT_H
#define RDYNAMICQOBJECT_H

#include "dynamicqobject.h"

#include "MocMethod.hpp"

typedef struct SEXPREC* SEXP;

class RDynamicQObject : public DynamicQObject {
public:
  RDynamicQObject(const MocMethod &method, SEXP function, SEXP userData,
                  QObject *sender);
  virtual ~RDynamicQObject();
  
  virtual DynamicSlot *createSlot(const char *slot);

  const MocMethod & method() { return _method; }
  SEXP function() { return _function; }
  SEXP userData() { return _userData; }
  
private:
  MocMethod _method;
  SEXP _function;
  SEXP _userData;
};

class RDynamicSlot : public DynamicSlot {
public:
  RDynamicSlot(RDynamicQObject *object) : object(object) { }
  virtual void call(QObject *sender, void **arguments);
private:
  RDynamicQObject *object;
};

#endif
