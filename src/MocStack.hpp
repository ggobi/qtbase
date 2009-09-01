#ifndef MOC_STACK_H
#define MOC_STACK_H

#include <QVector>

#include <smoke.h>

class SmokeStack;
class SmokeType;

/* High-level wrapper around the void** representing the Moc
   stack. Translates between the Moc and Smoke stacks.  */

class MocStack {
public:
  MocStack(void **o, int size);
  MocStack(const SmokeStack &smoke, QVector<SmokeType> types);

  inline void ** items() const { return _o; }
  
  SmokeStack toSmoke(QVector<SmokeType> types);

  void returnToSmoke(const SmokeStack &stack, const SmokeType &type);
  
  void returnFromSmoke(const SmokeStack &stack, const SmokeType &type);

private:

  void setFromSmoke(Smoke::Stack stack, QVector<SmokeType> types);
  void setSmokeItem(Smoke::StackItem *item, void *o, const SmokeType &type);
  void setSmoke(Smoke::Stack stack, QVector<SmokeType> types);

  int _size;
  void **_o;
};

#endif
