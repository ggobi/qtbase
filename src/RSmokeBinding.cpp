#ifndef R_SMOKE_BINDING_H
#define R_SMOKE_BINDING_H

#include <QMetaObject>
#include <QMetaMethod>

#include "RSmokeBinding.hpp"
#include "SmokeObject.hpp"
#include "Class.hpp"
#include "DynamicBinding.hpp"
#include "SmokeMethod.hpp"

#include <Rinternals.h>

typedef struct SEXPREC* SEXP;

/* Thoughts about moving to SMOKE:

   Each object needs to be associated with a Smoke metadata blob, via
   the SmokeObject class.
   
   Memory management: Currently use reference counting, and handle
   different memory management policies via polymorphism. Smoke gives
   us type information, so we can look up policy hooks that are
   registered by type. Unlike extending Reference, this mechanism
   works across packages. Reference counting could be avoided by a
   hash from pointer to externalptr, as Luke suggested. However, we
   also need reference counting between Qt objects, like between views
   and scenes, although this is broken anyway, because the views can
   switch scenes without us knowing. We need to listen for view
   destruction and delete the scene if (a) R does not reference it and
   (b) no other views reference it. The same needs to be done for all
   MVC designs in Qt. Alternatively, we could just say that views do
   not own models in Qt, so the user must maintain model references.
 */

void RSmokeBinding::deleted(Smoke::Index classId, void *obj) {
  SmokeObject *o = SmokeObject::fromPtr(obj, smoke, classId);
#ifdef DEBUG
  qDebug("%p->~%s()", ptr, smoke->className(classId));
#endif
  if (!o) {
    return;
  }
  delete o;
}

/* We catch all qt_metacall invocations */
int
RSmokeBinding::qt_metacall(SmokeObject *so, QMetaObject::Call _c, int id,
                           void **_o)
{
  // Assume the target slot is a C++ one
  Smoke::StackItem i[4];
  i[1].s_enum = _c;
  i[2].s_int = id;
  i[3].s_voidp = _o;
  so->invokeMethod("qt_metacall$$?", i);
  int ret = i[0].s_int;
  if (ret < 0) {
    return ret;
  }
  
  if (_c != QMetaObject::InvokeMetaMethod)
    return id;
  
  QObject * qobj = reinterpret_cast<QObject *>(so->castPtr("QObject"));
  // get obj metaobject with a virtual call
  const QMetaObject *metaobject = qobj->metaObject();

  // get method/property count
  int count = 0;
  if (_c == QMetaObject::InvokeMetaMethod) {
    count = metaobject->methodCount();
  } else {
    count = metaobject->propertyCount();
  }

  if (_c == QMetaObject::InvokeMetaMethod) {
    QMetaMethod method = metaobject->method(id);
    if (method.methodType() == QMetaMethod::Signal) {
      metaobject->activate(qobj, id, (void**) _o);
      return id - count;
    }
    /* TODO
    FromMocMethodCall slot(o->smoke(), qobj->metaObject(), id, self, _o);
    slot.marshal();
    */
  }
  
  return id - count;
}

bool RSmokeBinding::callMethod(Smoke::Index method, void *obj,
                               Smoke::Stack args, bool isAbstract)
{
  SmokeObject *o =
    SmokeObject::fromPtr(obj, smoke, smoke->methods[method].classId);

#ifdef DEBUG
    Smoke::Method & meth = smoke->methods[method];
    QByteArray signature(smoke->methodNames[meth.name]);
    signature += "(";
    for (int i = 0; i < meth.numArgs; i++) {
      if (i != 0) signature += ", ";
      signature += smoke->types[smoke->argumentList[meth.args + i]].name;
    }
    signature += ")";
    if (meth.flags & Smoke::mf_const) {
      signature += " const";
    }
    qDebug("module: %s virtual %p->%s::%s called", smoke->moduleName(),
           o->ptr(), smoke->classes[smoke->methods[method].classId].className,
           (const char *) signature);
#endif

  const char *methodName = smoke->methodNames[smoke->methods[method].name];
  
  /* TODO: instead, have MocClass provide a hard-coded 'qt_metacall'
     method to which we implicitly delegate.
  if (!qstrcmp(methodName, "qt_metacall") && o->instanceOf("QObject"))
    qt_metacall(o, (QMetaObject::Call)args[1].s_enum, args[2].s_int,
                (void **)args[3].s_voidp);
  */
  
  const Class *c = o->klass();
  bool success = false;
  bool impl = false;
  QList<const Class *> p = c->parents();
  while(p.size() == 1 && c->smokeBase() == p[0]->smokeBase() && !impl) {
    impl = c->implementsMethod(methodName);
    c = p[0];
    p = c->parents();
  }
  if (impl) {
    //qDebug("user implements: %s", methodName);
    DynamicBinding binding(SmokeMethod(smoke, method));
    binding.invoke(o, args);
    success = binding.lastError() == Method::NoError;
    if (!success)
      warning("Virtual method invocation failed for %s::%s", c->name(),
              methodName);
  }
  if (!success && isAbstract)
    warning("Class '%s' does not implement pure virtual '%s'", c->name(),
            methodName);
  return success;
}
 
char * RSmokeBinding::className(Smoke::Index classId) {
  return (char*) smoke->classes[classId].className;
}

#endif
