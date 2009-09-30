#ifndef SMOKE_OBJECT_H
#define SMOKE_OBJECT_H

#include <QHash>
#include <smoke.h>

class RQtModule;
class Class;

// To avoid conflicts between Qt and R headers
typedef struct SEXPREC* SEXP;

/* A SmokeObject is a pairing of a C++ pointer and a SEXP, both
   representing the instance of a class. It maintains a link to its
   Class, as well as some information about memory ownership.

   It also happens to define some Smoke-specific utilities. We have
   not yet found the need to make these more general.
*/

class SmokeObject {
    
public:

  ~SmokeObject();
  
  static SmokeObject *fromPtr(void *ptr, const Class *klass,
                              bool allocated = false, bool copy = false);
  static SmokeObject *fromPtr(void *ptr, Smoke *smoke, const char *name,
                              bool allocated = false, bool copy = false);
  static SmokeObject *fromPtr(void *ptr, Smoke *smoke, int classId,
                              bool allocated = false, bool copy = false);
  
  static SmokeObject *fromSexp(SEXP sexp);
  static SEXP sexpFromPtr(void *ptr, const Class *klass,
                                 bool allocated = false, bool copy = false);
  static SEXP sexpFromPtr(void *ptr, Smoke *smoke, const char *name,
                                 bool allocated = false, bool copy = false);

  /* Core behaviors */
  inline void *ptr() const { return _ptr; }
  SEXP sexp();
  SEXP internalSexp(SEXP env);
  void invalidateSexp();
  void invalidateInternalTable();
  SEXP fieldEnv() const;
  
  inline const Class *klass() const { return _klass; }
  inline bool allocated() const { return _allocated; }
  bool memoryIsOwned() const;

  /* Various utilities */
  Smoke *smoke() const;
  int classId() const;
  const char *className() const;
  RQtModule *module() const;
  void invokeMethod(const char *name, Smoke::Stack stack = NULL);
  void * clonePtr() const;
  SmokeObject *clone() const;
  void * castPtr(const char *className) const;
  void cast(const Class *klass);
  bool instanceOf(const char *className) const;
  SEXP enclose(SEXP fun);
  
private:

  void orphanTable(SEXP sexp) const;
  SEXP internalTable();
  void maybeDestroy();
  void castSexp(SEXP sexp);
  SEXP createSexp(SEXP parentEnv);
  
  void *_ptr;
  const Class *_klass;
  bool _allocated;
  SEXP _sexp;  
  SEXP _internalTable;
  mutable SEXP _fieldEnv;
  
  static QHash<void *, SmokeObject *> instances;

  SmokeObject(void *ptr, const Class *klass, bool allocated = false);
};

#endif
