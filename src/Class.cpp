#include <smoke.h>
#include <QMetaObject>

#include "RClass.hpp"
#include "ClassFactory.hpp"
#include "SmokeObject.hpp"
#include "InstanceObjectTable.hpp"
#include "smoke.hpp"
#include "SmokeType.hpp"

#include <Rinternals.h>

ClassFactory *Class::_classFactory = NULL;
QHash<QByteArray, const Class *> Class::_classMap;

ClassFactory *Class::classFactory() {
  if (!_classFactory) _classFactory = new ClassFactory;
  return _classFactory;
}

const Class* Class::fromSmokeId(Smoke *smoke, int classId) {
  const char *name = smoke->classes[classId].className;
  const Class *klass = _classMap[name];
  if (!klass) {
    if (smoke->classes[classId].external) { // ensure we have actual class
      Smoke::ModuleIndex mi = Smoke::classMap[name];
      if (mi.index) {
        smoke = mi.smoke;
        classId = mi.index;
      } // else we have a ghost class; we have done the best we could
    }
    klass = classFactory()->createClass(smoke, classId);
    _classMap[name] = klass;
  }
  return klass;
}
const Class* Class::fromSmokeType(const SmokeType &type) {
  return fromSmokeId(type.smoke(), type.classId());
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
const Class* Class::fromName(const char *name) {
  const Class *klass = _classMap[name];
  if (!klass)
    klass = fromSmokeName(NULL, name);
  return klass;
}
const Class* Class::fromMetaObject(const QMetaObject *meta) {
  const Class *klass = _classMap[meta->className()];
  if (!klass) {
    klass = classFactory()->createClass(meta);
    _classMap[meta->className()] = klass;
  }
  return klass;
}

const Class* Class::fromSexp(SEXP sexp, bool forceNew) {
  static SEXP nameSym = install("name");
  const Class *klass = NULL;
  if (inherits(sexp, "RQtClass")) {
    const char *name = CHAR(asChar(getAttrib(sexp, nameSym)));
    klass = _classMap[name];
    // FIXME: forceNew will leak; who cares?
    if (!klass || forceNew) {
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

Method *Class::findImplicitConverter(const SmokeObject *source) const {
  // not very optimized, but should be fast enough
  QList<Method *> meths = methods(Method::Constructor | Method::Implicit);
  Method *coercer = NULL;
  foreach(Method *meth, meths) {
    QVector<SmokeType> types = meth->types();
    if (types.size() == 2 && source->instanceOf(types[1]))
      coercer = meth;
    else delete meth;
  }
  return coercer;
}

bool Class::operator==(const Class &b) const {
  const Class &a = *this;
  const char *aname = a.name();
  const char *bname = b.name();
  if(aname == bname) return true;
  if(aname && bname && qstrcmp(aname, bname) == 0)
    return true;
  return false;
}
