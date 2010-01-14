#include <smoke.h>

#include "RClass.hpp"
#include "ClassFactory.hpp"
#include "SmokeObject.hpp"
#include "InstanceObjectTable.hpp"
#include "smoke.hpp"

#include <Rinternals.h>

ClassFactory *Class::_classFactory = NULL;
QHash<const char *, const Class *> Class::_classMap;

const Class* Class::fromSmokeId(Smoke *smoke, int classId) {
  const char *name = smoke->classes[classId].className;
  const Class *klass = _classMap[name];
  if (!klass) {
    if (!_classFactory) _classFactory = new ClassFactory;
    klass = _classFactory->createClass(smoke, classId);
    _classMap[name] = klass;
  }
  return klass;
}
const Class* Class::fromSmokeName(Smoke *smoke, const char *name) {
  if (!smoke) {
    Smoke::ClassMap::iterator i = Smoke::classMap.find(name);
    if (i == Smoke::classMap.end()) {
      qCritical("No smoke found for class: %s", name);
      return NULL;
    } else smoke = i->second.smoke;
  }
  return fromSmokeId(smoke, smoke->idClass(name).index);
}

const Class* Class::fromSexp(SEXP sexp) {
  static SEXP nameSym = install("name");
  const Class *klass = NULL;
  if (inherits(sexp, "RQtClass")) {
    const char *name = CHAR(asChar(getAttrib(sexp, nameSym)));
    klass = _classMap[name];
    if (!klass) {
      if (inherits(sexp, "RQtSmokeClass"))
        klass = Class::fromSmokeName(NULL, name);
      else if (inherits(sexp, "RQtUserClass")) {
        klass = new RClass(sexp);
        _classMap[name] = klass;
      }
    }
  } else qCritical("Unknown R class representation");
  return klass;
}

/* Object methods */

QList<const Class *> Class::ancestors() const {
  QList<const Class *> _parents = parents();
  QList<const Class *> classes = _parents;
  const Class *parent;
  foreach(parent, _parents)
    classes.append(parent->ancestors());
  return classes;
}

InstanceObjectTable *Class::createObjectTable(SmokeObject *obj) const {
  return new InstanceObjectTable(obj);
}
