#ifndef R_QT_MODULE_H
#define R_QT_MODULE_H

#include <QHash>

#include <smoke.h>

#include "RSmokeBinding.hpp"

class SmokeList;
class SmokeObject;

typedef int (*ResolveClassIdFn)(const SmokeObject * so);
typedef bool (*MemoryIsOwnedFn)(const SmokeObject *so);

class RQtModule {
  
private:
  
  RSmokeBinding *_binding;
  ResolveClassIdFn _resolveClassId;
  MemoryIsOwnedFn _memoryIsOwned;
  
  static QHash<Smoke *, RQtModule *> modules;
  
public:

  RQtModule(RSmokeBinding *binding,
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
  
  static RQtModule *registerModule(RQtModule *module);
  static RQtModule *module(Smoke *smoke);
  static SmokeList smokes();
};

#endif