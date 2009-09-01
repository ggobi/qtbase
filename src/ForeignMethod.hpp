#ifndef FOREIGN_METHOD_H
#define FOREIGN_METHOD_H

#include "Method.hpp"

typedef struct SEXPREC* SEXP;

class ForeignMethod : public virtual Method {
public:
  virtual SEXP invoke(SEXP obj, SEXP args);
};

#endif
