#include "MethodCall.hpp"
#include "RMethod.hpp"
#include "SmokeMethod.hpp"
#include "SmokeObject.hpp"
#include "TypeHandler.hpp"

#include <Rinternals.h>

MethodCall::MethodCall(Method *method, SEXP obj, SEXP args, bool super)
  : _cur(0), _called(false), _mode(Identity), _super(super),
    _target(obj ? SmokeObject::fromSexp(obj) : NULL), _stack(NULL),
    _args(args), _ret(R_NilValue), _method(method), _types(method->types())
{ }
MethodCall::MethodCall(Method *method, SmokeObject *obj, Smoke::Stack args,
                       bool super)
  : _cur(0), _called(false), _mode(Identity), _super(super),
    _target(obj), _stack(args), _args(NULL), _ret(R_NilValue),
    _method(method), _types(method->types())
{ }
MethodCall::MethodCall(RMethod *method, SEXP obj, SEXP args, bool super)
  : _cur(0), _called(false), _mode(Identity), _super(super),
    _target(obj ? SmokeObject::fromSexp(obj) : NULL), _stack(NULL),
    _args(args), _ret(R_NilValue), _method(method), _types(method->types())
{ } 
MethodCall::MethodCall(ForeignMethod *method, SEXP obj, SEXP args, bool super)
  : _cur(0), _called(false), _mode(RToSmoke), _super(super),
    _target(obj ? SmokeObject::fromSexp(obj) : NULL), _stack(NULL),
    _args(args), _ret(R_NilValue), _method(method), _types(method->types())
{ }
MethodCall::MethodCall(RMethod *method, SmokeObject *obj, Smoke::Stack args,
                       bool super)
  : _cur(0), _called(false), _mode(SmokeToR), _super(super),
    _target(obj), _stack(args), _args(NULL), _ret(R_NilValue), 
    _method(method), _types(method->types())
{ }
MethodCall::MethodCall(ForeignMethod *method, SmokeObject *obj,
                       Smoke::Stack args, bool super)
  : _cur(0), _called(false), _mode(Identity), _super(super),
    _target(obj), _stack(args), _args(NULL), _ret(R_NilValue),
    _method(method), _types(method->types())
{ }

/* These two are in cpp, because we do not want Rinternals.h in header */
SEXP MethodCall::sexp() const {
  if (_cur) return VECTOR_ELT(_args, _cur-1);
  else return _ret;
}
void MethodCall::setSexp(SEXP sexp) {
  if (_cur) SET_VECTOR_ELT(_args, _cur-1, sexp);
  else _ret = sexp;
}

/* Keep SmokeObject out of header */
const Class *MethodCall::klass() const {
   const Class *c = _target ? _target->klass() : _method->klass();
   if (_super)
     c = c->parents()[0];
   return c;
}

void MethodCall::unsupported() {
  const char *role = _cur ?  "argument" : "return value";
  const char *dir = _mode == MethodCall::RToSmoke ? "from" : "to";
  error("Cannot convert %s of type '%s' %s SEXP in %s::%s", role,
        type().name(), dir, klass()->name(), _method->name());
}

int MethodCall::numArgs() const {
  return _args ? length(_args) : (stackSize() - 1);
}

void MethodCall::marshal() {
  int oldcur = _cur;
  _cur++; // handle arguments

  while(!_called && _cur < stackSize()) {
    marshalItem();
    _cur++;
  }

  if (!_called) {
    _called = true;
    _cur = 0;
    invokeMethod();
    if (_method->lastError() == Method::NoError) {
      flip();
      marshalItem();
      flip();
    }
  }

  _cur = oldcur;
}

void MethodCall::invokeMethod() {
  if (_mode == SmokeToR || (_mode == Identity && _args)) {
    _ret = _method->invoke(_target ? _target->sexp() : NULL, _args);
    // TODO: handle out parameters as a 'qreturn' object, distribute
    // to arguments. For other direction, RToSmoke, introduce a new
    // method, 'sexpReturn' that dynamically creates a 'qreturn'
    // object, if necessary. This requires tracking which arguments
    // the user wants returned.
  }
  else _method->invoke(_target, _stack);
}

/* Evaluation */

#undef eval

void MethodCall::eval() {
  if (_mode == RToSmoke) 
    _stack = new Smoke::StackItem[length(_args) + 1];
  else if (_mode == SmokeToR)
    PROTECT(_args = allocVector(VECSXP, stackSize() - 1));
  if (_mode != Identity) {
    marshal();
    _called = false;
  } else invokeMethod();
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


extern void marshal_basetype(MethodCall *m);
extern void marshal_void(MethodCall *m);
extern void marshal_unknown(MethodCall *m);

extern int scoreArg_basetype(SEXP arg, const SmokeType &type);
extern int scoreArg_unknown(SEXP arg, const SmokeType &type);

static TypeHandler baseHandler =
  { "base", marshal_basetype, scoreArg_basetype };
static TypeHandler unknownHandler =
  { "unknown", marshal_unknown, scoreArg_unknown };
static TypeHandler voidHandler =
  { "void", marshal_void, NULL };

TypeHandler *MethodCall::typeHandler(const SmokeType &type) {
  TypeHandler *h = NULL;
  if (type.elem())
    h = &baseHandler;
  else if (!type.name())
    h = &voidHandler;
  else h = typeHandlers[type.name()];
  if (!h)
    h = &unknownHandler;
  return h;
}

TypeHandler::MarshalFn MethodCall::marshalFn(const SmokeType &type) {
  return typeHandler(type)->marshalFn;
}

int MethodCall::scoreArg(SEXP arg, Smoke *smoke, Smoke::Index type) {
  return scoreArg(arg, SmokeType(smoke, type));
}

int MethodCall::scoreArg(SEXP arg, const SmokeType &type) {
  return scoreArgFn(type)(arg, type);
}

TypeHandler::ScoreArgFn MethodCall::scoreArgFn(const SmokeType &type) {
  return typeHandler(type)->scoreArgFn;
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
    SmokeObject *o = SmokeObject::fromSexp(arg);
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

// FIXME: this only works from R. Could generalize, but not sure if
// this caching is worth it yet.
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
