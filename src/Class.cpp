#include <smoke.h>

#include "Class.hpp"
#include "ClassFactory.hpp"
#include "smoke.hpp"

#include <Rinternals.h>

ClassFactory *Class::_classFactory = NULL;
QHash<const char *, Class *> Class::_classMap;

const Class* Class::fromSmokeId(Smoke *smoke, int classId) {
  const char *name = smoke->classes[classId].className;
  Class *klass = _classMap[name];
  if (!klass) {
    if (!_classFactory) _classFactory = new ClassFactory;
    klass = _classFactory->createClass(smoke, classId);
    _classMap[name] = klass;
  }
  return klass;
}
const Class* Class::fromSmokeName(Smoke *smoke, const char *name) {
  return fromSmokeId(smoke, smoke->idClass(name).index);
}

// TODO: R user classes
const Class* Class::fromSexp(SEXP sexp) {
  static SEXP nameSym = install("name");
  static SEXP smokeSym = install("smoke");
  if (TYPEOF(sexp) == CLOSXP) {
    const char * className = CHAR(asChar(getAttrib(sexp, nameSym)));
    Smoke *smoke = asSmoke(getAttrib(sexp, smokeSym));
    return Class::fromSmokeName(smoke, className);
  }
  return NULL;
}

QList<const Class *> Class::ancestors() const {
  QList<const Class *> _parents = parents();
  QList<const Class *> classes = _parents;
  const Class *parent;
  foreach(parent, _parents)
    classes.append(parent->ancestors());
  return classes;
}
