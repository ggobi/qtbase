#ifndef MOC_INVOKABLE_H
#define MOC_INVOKABLE_H

class QObject;

#include "Method.hpp"

class MocInvokable : public virtual Method {
  virtual void invoke(QObject *obj, void **o) = 0;
};

#endif
