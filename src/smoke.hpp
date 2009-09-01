#ifndef SMOKE_HPP
#define SMOKE_HPP

class Smoke;
typedef struct SEXPREC* SEXP;

Smoke *asSmoke(SEXP rsmoke);
SEXP asRSmoke(Smoke *smoke);

#endif
