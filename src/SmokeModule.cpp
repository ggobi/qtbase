#include "SmokeModule.hpp"
#include "SmokeList.hpp"

QHash<Smoke *, SmokeModule *> SmokeModule::modules;

SmokeModule *SmokeModule::registerModule(SmokeModule *module) {
  modules[module->smoke()] = module;
  return module;
}

SmokeModule *SmokeModule::module(Smoke *smoke) {
  return modules[smoke];
}

SmokeList SmokeModule::smokes() {
  return SmokeList(modules.keys());
}
