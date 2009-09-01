#ifndef TYPE_HANDLER_H
#define TYPE_HANDLER_H

class MethodCall;
class SmokeType;

typedef struct SEXPREC* SEXP;

struct TypeHandler {
  typedef void (*MarshalFn)(MethodCall *);
  typedef int (*ScoreArgFn)(SEXP arg, const SmokeType &type);

  const char *name;
  MarshalFn marshalFn;
  ScoreArgFn scoreArgFn;
};

#endif
