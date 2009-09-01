#ifndef MOC_METHOD_H
#define MOC_METHOD_H

#include <QVector>
#include <QMetaMethod>

#include <smoke.h>

#include "ForeignMethod.hpp"
#include "MocInvokable.hpp"

class QMetaObject;
class Smoke;
class SmokeType;
class SmokeObject;

/* Tie QMetaMethod into the framework. */
class MocMethod : public ForeignMethod, public MocInvokable {
private:
  QMetaMethod _method;
  const QMetaObject *_meta;
  int _id;
  Smoke *_smoke;
  
public:
  MocMethod(Smoke *smoke, const QMetaObject *meta, int id)
    : _method(meta->method(id)), _meta(meta), _id(id), _smoke(smoke) { }
  
  inline QMetaMethod metaMethod() const { return _method; }
  inline const QMetaObject *metaObject() const { return _meta; }
  inline int id() const { return _id; }

  virtual Smoke* smoke() const { return _smoke; }
  virtual const char *name() const { return _method.signature(); }
  virtual const Class* klass() const;
  virtual QVector<SmokeType> types() const;
  virtual Qualifiers qualifiers() const {
    Qualifiers flags = 0;
    if (_method.access() == QMetaMethod::Protected)
      flags |= Protected;
    return flags;
  }

  virtual void invoke(SmokeObject *o, Smoke::Stack stack);
  virtual void invoke(QObject *obj, void **o);
  
private:
  Smoke::Index smokeTypeForName(Smoke *smoke, QByteArray name) const;
};

#endif
