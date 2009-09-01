#include "MethodCall.hpp"
#include "RMethod.hpp"
#include "SmokeMethod.hpp"
#include "SmokeObject.hpp"
#include "TypeHandler.hpp"

#include <Rinternals.h>

MethodCall::MethodCall(Method *method, SEXP obj, SEXP args)
  : _cur(0), _called(false), _mode(Identity),
    _target(SmokeObject::fromExternalPtr(obj)), _stack(NULL), _args(args),
    _method(method), _types(method->types())
{ }
MethodCall::MethodCall(Method *method, SmokeObject *obj, Smoke::Stack args)
  : _cur(0), _called(false), _mode(Identity),
    _target(obj), _stack(args), _args(NULL),
    _method(method), _types(method->types())
{ }
MethodCall::MethodCall(RMethod *method, SEXP obj, SEXP args)
  : _cur(0), _called(false), _mode(Identity),
    _target(SmokeObject::fromExternalPtr(obj)), _stack(NULL), _args(args),
    _method(method), _types(method->types())
{ } 
MethodCall::MethodCall(ForeignMethod *method, SEXP obj, SEXP args)
  : _cur(0), _called(false), _mode(RToSmoke),
    _target(SmokeObject::fromExternalPtr(obj)), _stack(NULL), _args(args),
    _method(method), _types(method->types())
{ }
MethodCall::MethodCall(RMethod *method, SmokeObject *obj, Smoke::Stack args)
  : _cur(0), _called(false), _mode(SmokeToR),
    _target(obj), _stack(args), _args(NULL),
    _method(method), _types(method->types())
{ }
MethodCall::MethodCall(ForeignMethod *method, SmokeObject *obj,
                       Smoke::Stack args)
  : _cur(0), _called(false), _mode(Identity),
    _target(obj), _stack(args), _args(NULL),
    _method(method), _types(method->types())
{ }

/* These two are in cpp, because we do not want Rinternals.h in header */
SEXP MethodCall::sexp() const {
  if (_cur) return VECTOR_ELT(_args, _cur-1);
  else return _sret;
}
void MethodCall::setSexp(SEXP sexp) {
  if (_cur) SET_VECTOR_ELT(_args, _cur-1, sexp);
  else _sret = sexp;
}

/* Keep SmokeObject out of header */
const Class *MethodCall::klass() const {
   return _target ? _target->klass() : _method->klass();
}

void MethodCall::unsupported() {
  const char *role = _cur ?  "argument" : "return value";
  const char *dir = _mode == MethodCall::RToSmoke ? "from" : "to";
  error("Cannot convert %s of type '%s' %s SEXP in %s::%s", role,
        type().name(), dir, klass()->name(), _method->name());
}

void MethodCall::marshal() {
  int oldcur = _cur;
  _cur++; // handle arguments
  while(!_called && _cur < stackSize()) {
    handle();
    _cur++;
  }

  if (!_called) {
    _called = true;
    _cur = 0;
    if (_mode == SmokeToR || (_mode == Identity && _args))
      _method->invoke(_target->externalPtr(), _args);
    else _method->invoke(_target, _stack);
    if (_method->lastError() == Method::NoError) {
      flip();
      handle();
      flip();
    }
  }

  _cur = oldcur;
}
  
/* Evaluation */

#undef eval

void MethodCall::eval() {
  if (_mode == RToSmoke) 
    _stack = new Smoke::StackItem[length(_args) + 1];
  else if (_mode == SmokeToR)
    PROTECT(_args = allocVector(VECSXP, stackSize() - 1));
  marshal();
  if (_mode == RToSmoke) {
    delete[] _stack;
    _stack = NULL;
  } else if (_mode == SmokeToR) {
    UNPROTECT(1);
    _args = NULL;
  }
}

/* Static MethodCall functions (TypeHandler registry) */

QHash<QByteArray, TypeHandler *> MethodCall::typeHandlers;

void MethodCall::registerTypeHandlers(TypeHandler *handlers) {
  while(handlers->name) {
    typeHandlers.insert(handlers->name, handlers);
    handlers++;
  }
}

TypeHandler *MethodCall::typeHandler(const SmokeType &type) {
  TypeHandler *h = typeHandlers[type.name()];	
  if (h == 0 && type.isConst() && strlen(type.name()) > strlen("const ")) {
    h = typeHandlers[type.name() + strlen("const ")];
  }	
  return h;
}

TypeHandler::MarshalFn marshal_basetype;
TypeHandler::MarshalFn marshal_void;
TypeHandler::MarshalFn marshal_unknown;

TypeHandler::MarshalFn MethodCall::marshalFn(const SmokeType &type) {
  if (type.elem())
    return marshal_basetype;
  if (!type.name())
    return marshal_void;
  TypeHandler *h = typeHandler(type);
  if (h != 0) {
    return h->marshalFn;
  }
  return marshal_unknown;
}

int MethodCall::scoreArg(SEXP arg, Smoke *smoke, Smoke::Index type) {
  return scoreArg(arg, SmokeType(smoke, type));
}

int MethodCall::scoreArg(SEXP arg, const SmokeType &type) {
  return scoreArgFn(type)(arg, type);
}

TypeHandler::ScoreArgFn scoreArg_basetype;
TypeHandler::ScoreArgFn scoreArg_unknown;

TypeHandler::ScoreArgFn MethodCall::scoreArgFn(const SmokeType &type) {
  if (type.elem())
    return scoreArg_basetype;
  TypeHandler *h = typeHandler(type);
  if (h != 0) {
    return h->scoreArgFn;
  }
  return scoreArg_unknown;
}

/* Cache key generation */

QByteArray
MethodCall::argKey(SEXP arg) const
{
  const char * className = CHAR(asChar(getAttrib(arg, R_ClassSymbol)));
  const char *r = "";
  if (arg == R_NilValue)
    r = "u";
  else if (TYPEOF(arg) == INTSXP) {
    if (inherits(arg, "QtEnum"))
      r = className;
    else r = "i";
  } else if (TYPEOF(arg) == REALSXP)
    r = "n";
  else if (TYPEOF(arg) == STRSXP)
    r = "s";
  else if(TYPEOF(arg) == LGLSXP)
    r = "B";
  else if (TYPEOF(arg) == EXTPTRSXP) {
    SmokeObject *o = SmokeObject::fromExternalPtr(arg);
    if (o == 0 || o->smoke() == 0) {
      r = "a";
    } else {
      r = o->smoke()->classes[o->classId()].className;
    }
  } else {
    r = "U";
  }
  return QByteArray(r);
}

// NOTE: the assumption is that class name uniquely identifies a class
QByteArray
MethodCall::cacheKey() const {
  QByteArray mcid(klass()->name());
  mcid += ';';
  mcid += _method->name();
  for(int i = 0; i < length(_args); i++) {
    mcid += ';';
    mcid += argKey(VECTOR_ELT(_args, i));
  }
  return mcid;
}
