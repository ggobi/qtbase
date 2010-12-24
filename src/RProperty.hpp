#ifndef R_PROPERTY_H
#define R_PROPERTY_H

#include <QMetaProperty>

#include "SmokeType.hpp"
#include "Property.hpp"

class RProperty : public Property {
public:
  RProperty(const char *name, const SmokeType &type, SEXP reader, SEXP writer)
    : _name(name), _reader(reader), _writer(writer), _type(type) { }
  
  virtual const char *name() const {
    return _name;
  }
  
  virtual SEXP read(SEXP obj) const;
  virtual Smoke::StackItem read(SmokeObject *so) const;

  virtual bool write(SEXP obj, SEXP val);
  virtual bool write(SmokeObject *so, const Smoke::StackItem &item);

  virtual bool isReadable() const;
  virtual bool isWritable() const;

private:
  QByteArray _name;
  SEXP _reader, _writer;
  SmokeType _type;
};

#endif
