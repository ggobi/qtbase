#include <qt_smoke.h>

#include "ClassFactory.hpp"
#include "SmokeClass.hpp"
#include "MocClass.hpp"

Class *ClassFactory::createClass(Smoke *smoke, int classId) {
  static int QObjectId = qt_Smoke->idClass("QObject").index;
  Class *klass = NULL;
  if (classId > 0 && classId <= smoke->numClasses) {
    klass = new SmokeClass(smoke, classId);
    if (smoke->isDerivedFrom(smoke, classId, qt_Smoke, QObjectId)) {
      klass = new MocClass(klass);
    }
  }
  return klass;
}

Class *ClassFactory::createClass(const QMetaObject *meta) {
  return new MocClass(meta);
}
