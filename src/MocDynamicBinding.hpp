#ifndef MOC_DYNAMIC_BINDING_H
#define MOC_DYNAMIC_BINDING_H

#include "DynamicBinding.hpp"
#include "MocInvokable.hpp"

class QObject;
class MocMethod;

/* Provides a Moc friendly wrapper that delegates to Smoke */
class MocDynamicBinding : public DynamicBinding, public MocInvokable {
public:
  MocDynamicBinding(const MocMethod &method);
  virtual void invoke(QObject *obj, void **stack);
};

#endif
