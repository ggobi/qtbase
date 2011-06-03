#ifndef TYPE_HANDLER_H
#define TYPE_HANDLER_H

class MethodCall;
class SmokeType;

typedef struct SEXPREC* SEXP;

/* A simple structure for packaging the logic for handling a specific
   type. The logic is encapsulated in pointers to C functions, so that
   we can pass them from other packages to extend the type handling.
   This is what prevents us from using a class template here.
*/
struct TypeHandler {
  typedef void (*MarshalFn)(MethodCall *);
  typedef int (*ScoreArgFn)(SEXP arg, const SmokeType &type);

  const char *name;
  MarshalFn marshalFn;
  ScoreArgFn scoreArgFn;
};

#endif
