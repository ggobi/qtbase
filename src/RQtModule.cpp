#include "RQtModule.hpp"
#include "SmokeList.hpp"

QHash<Smoke *, RQtModule *> RQtModule::modules;

RQtModule *RQtModule::registerModule(RQtModule *module) {
  modules[module->smoke()] = module;
  return module;
}

RQtModule *RQtModule::module(Smoke *smoke) {
  return modules[smoke];
}

SmokeList RQtModule::smokes() {
  return SmokeList(modules.keys());
}
