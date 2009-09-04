#ifndef OBJECT_TABLE_H
#define OBJECT_TABLE_H

#include <QList>

#include <R_ext/Boolean.h>

typedef struct SEXPREC* SEXP;

class ObjectTable {
public:

  ObjectTable() : _sexp(NULL) { }
  
  virtual ~ObjectTable();
  
  /* R_ObjectTable interface */
  virtual Rboolean exists(const char * name, Rboolean *canCache) = 0;
  virtual SEXP get(const char * name, Rboolean *canCache) = 0;
  virtual int remove(const char * name) = 0;
  virtual SEXP assign(const char * name, SEXP value) = 0;
  virtual SEXP objects() = 0;

  SEXP sexp(); // get wrapped sexp (externalptr for hashtab)

  static ObjectTable * fromSexp(SEXP sexp); // unwrap

protected:
  virtual QList<QByteArray> sexpClasses() const;

private:
  SEXP createSexp();
  SEXP _sexp;
};

#endif
