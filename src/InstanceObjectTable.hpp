#ifndef INSTANCE_OBJECT_TABLE_H
#define INSTANCE_OBJECT_TABLE_H

#include "ObjectTable.hpp"

class SmokeObject;

/* Low-level R wrapper on top of an instance. */

/* Provides symbols for non-static methods.
   TODO: enum values, properties, super method invocation
   NOTE: While C++/Java allow static invocations on instances, we
   only allow them from inside a class.
   FIXME: what about returning methods implemented in R directly,
   rather than a wrapper around qinvoke()?
*/
class InstanceObjectTable : public ObjectTable {
public:
  InstanceObjectTable(SmokeObject *instance)
    : _instance(instance), _internal(false) { }
  virtual ~InstanceObjectTable();
  
  inline SmokeObject *instance() { return _instance; }
  inline void setInstance(SmokeObject *instance) { _instance = instance; }
  
  inline bool internal() { return _internal; }
  inline void setInternal(bool internal) { _internal = internal; }
  
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
  SEXP enumValue(const char *name);
  SEXP fieldEnv();
  void checkInstance();
  bool methodExists(const char *name);
  
  SmokeObject *_instance;
  bool _internal;
};

#endif
