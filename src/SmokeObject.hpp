#ifndef SMOKE_OBJECT_H
#define SMOKE_OBJECT_H

#include <QHash>
#include <smoke.h>

class RQtModule;
class Class;

// To avoid conflicts between Qt and R headers
typedef struct SEXPREC* SEXP;

class SmokeObject {
  
private:

  void *_ptr;
  Class *_klass;
  bool _allocated;
  
  SEXP createExternalPtr() const;
  
  static QHash<const SmokeObject *, SEXP> externalPtrs;
  static QHash<void *, SmokeObject *> instances;

  SmokeObject(void *ptr, Class *klass, bool allocated = false);
  
public:

  ~SmokeObject();
  
  static SmokeObject *fromPtr(void *ptr, Class *klass, bool allocated = false,
                              bool copy = false);
  static SmokeObject *fromPtr(void *ptr, Smoke *smoke, const char *name,
                              bool allocated = false, bool copy = false);
  static SmokeObject *fromPtr(void *ptr, Smoke *smoke, int classId,
                              bool allocated = false, bool copy = false);
  
  static SmokeObject *fromExternalPtr(SEXP externalPtr);
  static SEXP externalPtrFromPtr(void *ptr, Class *klass,
                                 bool allocated = false, bool copy = false);
  static SEXP externalPtrFromPtr(void *ptr, Smoke *smoke, const char *name,
                                 bool allocated = false, bool copy = false);
  
  inline void *ptr() const { return _ptr; }
  inline bool allocated() const { return _allocated; }
  inline Class *klass() const { return _klass; }

  Smoke *smoke() const;
  int classId() const;
  SEXP externalPtr() const;
  RQtModule *module() const;
  bool memoryIsOwned() const;
  void invokeMethod(const char *name, Smoke::Stack stack = NULL);
  void * constructCopy();
  
  void * cast(const char *className) const;
  bool instanceOf(const char *className) const;
  
private:

};

#endif
