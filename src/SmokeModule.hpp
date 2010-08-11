#ifndef R_QT_MODULE_H
#define R_QT_MODULE_H

#include <QHash>

#include <smoke.h>

#include "RSmokeBinding.hpp"

class SmokeList;
class SmokeObject;

typedef int (*ResolveClassIdFn)(const SmokeObject * so);
typedef bool (*MemoryIsOwnedFn)(const SmokeObject *so);

class SmokeModule {
  
private:
  
  RSmokeBinding *_binding;
  ResolveClassIdFn _resolveClassId;
  MemoryIsOwnedFn _memoryIsOwned;
  
  static QHash<Smoke *, SmokeModule *> modules;
  
public:

  SmokeModule(RSmokeBinding *binding,
            ResolveClassIdFn resolveClassId, MemoryIsOwnedFn memoryIsOwned)
    : _binding(binding), _resolveClassId(resolveClassId),
      _memoryIsOwned(memoryIsOwned) { }

  SmokeBinding *binding() { return _binding; }
  const char *name() { return smoke()->moduleName(); }
  Smoke *smoke() { return _binding->getSmoke(); }
  
  int resolveClassId(const SmokeObject *so) {
    return _resolveClassId(so);
  }
  bool memoryIsOwned(const SmokeObject *so) {
    return _memoryIsOwned(so);
  }
  
  static SmokeModule *registerModule(SmokeModule *module);
  static SmokeModule *module(Smoke *smoke);
  static SmokeList smokes();
};

extern "C" Smoke* registerSmokeModule(Smoke *smoke);

#endif
