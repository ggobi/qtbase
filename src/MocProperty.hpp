#ifndef MOC_PROPERTY_H
#define MOC_PROPERTY_H

#include <QMetaProperty>

#include "Property.hpp"

class MocProperty : public Property {
public:
  MocProperty(const QMetaProperty &property) : _property(property) { }
  
  virtual const char *name() const {
    return _property.name();
  }
  
  virtual SEXP read(SEXP obj) const;
  virtual Smoke::StackItem read(SmokeObject *so) const;

  virtual bool write(SEXP obj, SEXP val);
  virtual bool write(SmokeObject *so, const Smoke::StackItem &item);

  virtual bool isReadable() const {
    return _property.isReadable();
  }
  virtual bool isWritable() const {
    return _property.isWritable();
  }

private:
  QMetaProperty _property;
};

#endif
