#ifndef INSTANCE_OBJECT_TABLE_H
#define INSTANCE_OBJECT_TABLE_H

#include "ObjectTable.hpp"

class SmokeObject;

/* Low-level R wrapper on top of an instance. */

/* Provides symbols for public, non-static methods.
   TODO: enum values, properties, protected toggle 
   NOTE: while C++/Java allow static invocations on instances, we
   avoid them, like C#.
*/
class InstanceObjectTable : public ObjectTable {
public:
  InstanceObjectTable(SmokeObject *instance) : _instance(instance) { }
  inline SmokeObject *instance() { return _instance; }

  virtual Rboolean exists(const char * name, Rboolean *canCache);
  virtual SEXP get(const char * name, Rboolean *canCache);
  virtual int remove(const char * name);
  virtual SEXP assign(const char * name, SEXP value);
  virtual SEXP objects();

  static SmokeObject *instanceFromSexp(SEXP sexp);

protected:
  virtual QList<QByteArray> sexpClasses() const;
  
private:

  SEXP methodClosure(const char *name);
    
  SmokeObject *_instance;
};

#endif
