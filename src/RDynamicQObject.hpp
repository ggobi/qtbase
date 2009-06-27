#ifndef RDYNAMICQOBJECT_H
#define RDYNAMICQOBJECT_H

#include <QList>
#include <QByteArray>
#include "dynamicqobject.h"

#include "wrappers.h"

class RDynamicQObject : public DynamicQObject {
public:
  RDynamicQObject(QList<QByteArray> paramTypes, QByteArray returnType,
                  SEXP function, SEXP userData, QObject *sender) :
    DynamicQObject(sender), _paramTypes(paramTypes), _returnType(returnType),
    _function(function), _userData(userData)
  {
    R_PreserveObject(function);
    if (userData)
      R_PreserveObject(userData);
  }
  virtual ~RDynamicQObject() {
    R_ReleaseObject(_function);
    if (_userData)
      R_ReleaseObject(_userData);
  }
  
  virtual DynamicSlot *createSlot(const char *slot);

  QList<QByteArray> paramTypes() { return _paramTypes; }
  QByteArray returnType() { return _returnType; }
  SEXP function() { return _function; }
  SEXP userData() { return _userData; }
  
private:
  // need to know the R function to call, the user data and the
  // argument types (and count)
  QList<QByteArray> _paramTypes;
  QByteArray _returnType;
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
