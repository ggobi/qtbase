#ifndef SMOKE_STACK_H
#define SMOKE_STACK_H

#include <smoke.h>

class SmokeStack {
public:
  SmokeStack(int size)
    : _size(size), _stack(new Smoke::StackItem[size]), _allocated(true) { }
  SmokeStack(Smoke::Stack stack, int size)
    : _size(size), _stack(stack), _allocated(false) { }
  
  virtual ~SmokeStack() {
    if (_allocated)
      delete[] _stack;
  }
  
  inline int size() const { return _size; }
  inline Smoke::Stack items() const { return _stack; }
  inline Smoke::Stack params() const { return _stack + 1; }
  inline Smoke::StackItem ret() const { return _stack[0]; }
  
private:
  int _size;
  Smoke::Stack _stack;
  bool _allocated;
};

#endif
