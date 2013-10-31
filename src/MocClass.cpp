#include "MocClass.hpp"
#include "MocMethod.hpp"
#include "MocProperty.hpp"
#include "MethodCall.hpp"
#include "SmokeObject.hpp"
#include "DynamicBinding.hpp"
#include "SmokeClass.hpp"

#include <Rinternals.h>

int /* search the meta data for a match to the name and args */
MocClass::findMethodId(Smoke *smoke, const QMetaObject *meta, const char *name,
                       SEXP args) const
{
  int result = -1;
  int classId = smoke->idClass(meta->className()).index;
  if (!args) return result; // Do not (yet?) support calls from Smoke
  while (classId == 0) {
    // go in reverse to choose most derived method first
    for (int id = meta->methodCount()-1; id >= 0; id--) {
      QMetaMethod meth = meta->method(id);
      if (meth.access() != QMetaMethod::Private) {
        QByteArray methodName = meth.name();
        // Don't check that the types of the R args match
        // the c++ ones for now,
        // only that the name and arg count is the same.
        if (methodName == name &&
            meth.parameterTypes().count() == length(args))
          result = id;
      }
    }
    meta = meta->superClass();
    classId = smoke->idClass(meta->className()).index;
  }
  
  return result;
}

Method*
MocClass::findMethod(const MethodCall& call) const {
  Method *method = _delegate->findMethod(call);
  if (method)
    return(method);
  SmokeObject *o = call.target();
  /* only QObjects have meta methods */
  if (o && o->ptr() && o->instanceOf("QObject") && o->klass() == this) {
    /* unwrap the call */
    QObject * qobject = reinterpret_cast<QObject *>(o->castPtr("QObject"));
    const QMetaObject * meta = qobject->metaObject();
    /* get the method id */
    int id = findMethodId(o->smoke(), meta, call.method()->name(), call.args());
    if (id >= 0)
      method = new MocMethod(o->smoke(), meta, id);
  }
  return method;
}

const QMetaObject *MocClass::findMetaObject() const {
  const QMetaObject *meta;
  DynamicBinding binding = DynamicBinding(_delegate, "staticMetaObject");
  Smoke::StackItem items[1];
  items[0].s_voidp = NULL;
  binding.invoke(NULL, items);
  meta = reinterpret_cast<const QMetaObject*>(items[0].s_voidp);
  return meta;
}

const Class *MocClass::findDelegateClass() const {
  const QMetaObject *meta = _meta;
  const Class *delegate = NULL;
  do {
    Smoke::ClassMap::iterator i = Smoke::classMap.find(meta->className());
    if (i != Smoke::classMap.end()) {
      Smoke *smoke = i->second.smoke;
      delegate = Class::fromSmokeId(smoke,
                                    smoke->idClass(meta->className()).index);
    } else {
      meta = meta->superClass();
    }
  } while(meta);
  if (!delegate)
    qCritical("Failed to find Smoke delegate for meta class '%s'\n",
              _meta->className());
  return delegate;
}

QList<Method *> MocClass::methods(Method::Qualifiers qualifiers) const {
  int n = _meta->methodCount();
  QList<Method *> methods = _delegate->methods(qualifiers);
  Smoke *smoke = smokeBase()->smoke();
  for (int i = 0; i < n; i++) {
    if (_meta->method(i).access() != QMetaMethod::Private)
      if ((MocMethod(smoke, _meta, i).qualifiers() & qualifiers) == qualifiers)
        methods << new MocMethod(smoke, _meta, i);
  }
  return methods;
}

bool
MocClass::hasMethod(const char *name, Method::Qualifiers qualifiers) const {
  if (_methods.isEmpty()) {
    QList<Method *> meths = methods();
    for (int i = 0; i < meths.size(); i++) {
      _methods.insert(meths[i]->name(), i + 1);
      delete meths[i];
    }
  }
  bool found = _delegate->hasMethod(name, qualifiers);
  /* Simple optimization: do not check for method if the delegate is
     a smoke class, because we will not have any more methods. */
  if (!found && _delegate != smokeBase()) {
    int index = _methods[name] - 1;
    if (index != -1) {
      MocMethod method(smokeBase()->smoke(), _meta, index);
      if((method.qualifiers() & qualifiers) == qualifiers)
        found = true;
    }
  }
  return found;
}

bool MocClass::implementsMethod(const char *name) const {
  bool impl = _delegate->implementsMethod(name);
  if (!impl)
    impl = _meta->indexOfMethod(name) >= _meta->methodOffset();
  return impl;
}

// yes, we could access the QMetaEnums, but is there a use case?
QHash<const char *, int> MocClass::enumValues() const {
  return _delegate->enumValues();
}

Property *MocClass::property(const char *name) const {
  int index = _meta->indexOfProperty(name);
  Property *prop = NULL;
  if (index >= 0)
    prop = new MocProperty(_meta->property(index));
  return prop;
}

const char* MocClass::name() const {
  return _meta->className();
}
