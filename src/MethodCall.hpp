#ifndef METHOD_CALL_H
#define METHOD_CALL_H

#include <QVector>
#include <QHash>

#include <smoke.h>

#include "SmokeType.hpp"
#include "TypeHandler.hpp"

class Method;
class SmokeObject;
class QByteArray;
class RMethod;
class ForeignMethod;
class Class;

/* The workhorse of the bindings. Describes and carries out a method
   call, including argument marshalling, method invocation, and
   marshalling the return value.
*/

class MethodCall {
public:

  enum Mode { Identity, RToSmoke, SmokeToR };

  /* These constructors are specifically overloaded to determine the
     mode for the marshalling. Too bad there is no constructor
     chaining in C++. */
  
  MethodCall(Method *method, SEXP obj, SEXP args, bool super = false);
  MethodCall(Method *method, SmokeObject *obj, Smoke::Stack args,
             bool super = false);
  MethodCall(RMethod *method, SEXP obj, SEXP args, bool super = false);
  MethodCall(ForeignMethod *method, SEXP obj, SEXP args, bool super = false);
  MethodCall(RMethod *method, SmokeObject *obj, Smoke::Stack args,
             bool super = false);
  MethodCall(ForeignMethod *method, SmokeObject *obj, Smoke::Stack args,
             bool super = false);
  
  /* General Accessors */
  
  inline Method* method() const {
    return _method;
  }
  inline SmokeObject * target() const { return _target; }
  inline Smoke::Stack stack() const {
    return _stack;
  }
  inline int stackSize() const {
    return _types.size();
  }
  inline SEXP args() const {
    return _args;
  }
  inline QVector<SmokeType> types() const { return _types; }
  const Class *klass() const;
  int numArgs() const; 
  inline bool super() const { return _super; }  
  
  /* Marshalling accessors */

  inline Mode mode() const {
    return _mode;
  }
  inline SmokeType type() const {
    return _types[_cur];
  }
  // FIXME: Returning a non-const reference is bad form.
  inline Smoke::StackItem &item() const {
    return _stack[_cur];
  }
  SEXP sexp() const;
  void setSexp(SEXP sexp);
  inline Smoke *smoke() const { return type().smoke(); }
  /* Clean-up when:
     (1) Smoke returns something that doesn't fit in StackItem OR
     (2) Passing arguments to Smoke where Smoke does not take
         ownership. In general, it is easier (though suboptimal) to
         allocate C++ data on the heap and free it later, rather than
         allocating two different ways. Note that Smoke cleans up when
         a virtual override returns something big.
  */
  inline bool cleanup() const {
    SmokeType t = type();
    return (_mode == RToSmoke && _cur && (t.isConst() || t.isStack())) ||
      (t.isStack() && !_cur && _mode == SmokeToR);
  }
  inline bool returning() const {
    return _cur == 0;
  }
  
  /* Iterate the marshalling */
  
  void marshal();
  
  /* Main entry point */

  virtual void eval();
      
  /* Utilities */
  
  void unsupported();
  QByteArray cacheKey() const;

  /* TypeHandler registration and utilities */
  
  static void registerTypeHandlers(TypeHandler *handlers);
  static int scoreArg(SEXP arg, Smoke *smoke, Smoke::Index type);
  static int scoreArg(SEXP arg, const SmokeType &type);

private:

  static QHash<QByteArray, TypeHandler *> typeHandlers;
  static TypeHandler::MarshalFn marshalFn(const SmokeType &type);
  static TypeHandler::ScoreArgFn scoreArgFn(const SmokeType &type);

  static TypeHandler *typeHandler(const SmokeType &type);
  
  QByteArray argKey(SEXP arg) const;
  
  inline void flip() {
    if (_mode == RToSmoke)
      _mode = SmokeToR;
    else if (_mode == SmokeToR)
      _mode = RToSmoke;
  }

  inline void marshalItem() {
    TypeHandler::MarshalFn fn = marshalFn(type());
    (*fn)(this);
  }

  void invokeMethod();
  
  int _cur;
  bool _called;
  MethodCall::Mode _mode;
  bool _super;
  SmokeObject* _target;
  Smoke::Stack _stack;
  SEXP _args;
  SEXP _ret;
  Method* _method;
  QVector<SmokeType> _types;
};

#endif
