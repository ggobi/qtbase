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
  QByteArray _name;
  
public:
  MocMethod(Smoke *smoke, const QMetaObject *meta, int id);
  
  inline QMetaMethod metaMethod() const { return _method; }
  inline const QMetaObject *metaObject() const { return _meta; }
  inline int id() const { return _id; }

  virtual Smoke* smoke() const { return _smoke; }
  virtual const char *name() const { return _name.constData(); }
  virtual const Class* klass() const;
  virtual QVector<SmokeType> types() const;
  virtual Qualifiers qualifiers() const {
    Qualifiers flags = NotStatic | NotPrivate;
    if (_method.access() == QMetaMethod::Protected)
      flags |= Protected;
    else flags |= Public;
    return flags;
  }

  virtual void invoke(SmokeObject *o, Smoke::Stack stack);
  virtual void invoke(QObject *obj, void **o);
  
private:
  Smoke::Index smokeTypeForName(Smoke *smoke, QByteArray name) const;
};

#endif
