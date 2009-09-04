#include "ObjectTable.hpp"
#include "wrap.hpp"

#include <R_ext/Callbacks.h>

#define THIS (reinterpret_cast<ObjectTable *>(tb->privateData))

static void finalizeObjectTable(SEXP obj) {
  delete ObjectTable::fromSexp(obj);
}

Rboolean 
ObjectTable_exists(const char * const name, Rboolean *canCache,
                   R_ObjectTable *tb)
{
  return THIS->exists(name, canCache);
}

static SEXP
ObjectTable_get(const char * const name, Rboolean *canCache, R_ObjectTable *tb)
{
  return THIS->get(name, canCache);
}

static int
ObjectTable_remove(const char * const name,  R_ObjectTable *tb)
{
  return THIS->remove(name);
}

static SEXP
ObjectTable_assign(const char * const name, SEXP value, R_ObjectTable *tb)
{
  return THIS->assign(name, value);
}

static SEXP
ObjectTable_objects(R_ObjectTable *tb)
{
  return THIS->objects();
}

ObjectTable *ObjectTable::fromSexp(SEXP sexp) {
  R_ObjectTable *tb = unwrapPointerSep(sexp, ObjectTable, R_ObjectTable);
  return THIS;
}

SEXP ObjectTable::sexp() {
  if (!_sexp)
    _sexp = createSexp();
  return _sexp;
}

SEXP ObjectTable::createSexp() {
  R_ObjectTable *tb = new R_ObjectTable;
  tb->type = 15; // ?
  tb->cachedNames = NULL; // ?
  tb->active = TRUE;
    
  tb->privateData = this;
    
  tb->exists = ObjectTable_exists;
  tb->get = ObjectTable_get;
  tb->remove = ObjectTable_remove;
  tb->assign = ObjectTable_assign;
  tb->objects = ObjectTable_objects;
    
  tb->canCache = NULL;
  tb->onAttach = NULL;
  tb->onDetach = NULL;

  return wrapPointer(tb, sexpClasses(), finalizeObjectTable);
}

QList<QByteArray> ObjectTable::sexpClasses() const {
  QList<QByteArray> classes;
  classes.append("ObjectTable");
  classes.append("UserDefinedDatabase");
  return classes;
}

ObjectTable::~ObjectTable() {
  SEXP extptr = sexp();
  R_ObjectTable *tb =
    reinterpret_cast<R_ObjectTable *>(R_ExternalPtrAddr(extptr));
  delete tb;
  R_ClearExternalPtr(extptr);
}
