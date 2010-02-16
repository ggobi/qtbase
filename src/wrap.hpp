#ifndef WRAPPERS_H
#define WRAPPERS_H

#include <QList>

#include <R.h>
#include <Rinternals.h>

#define checkPointer(x, type) ({                                       \
      if (TYPEOF(x) != EXTPTRSXP)                                      \
        error("checkPointer: not an externalptr");                     \
      if (!inherits(x, #type))                                         \
        error("checkPointer: expected object of class '" #type "'");   \
    })

#define unwrapPointerSep(x, rtype, ctype) ({                            \
      checkPointer(x, rtype);                                           \
      reinterpret_cast<ctype *>(R_ExternalPtrAddr(x));                  \
    })

#define unwrapPointer(x, type) unwrapPointerSep(x, type, type)

void *_unwrapSmoke(SEXP x, const char *type);

#define unwrapSmoke(x, type) reinterpret_cast<type *>(_unwrapSmoke(x, #type))

SEXP wrapPointer(void *ptr,
                 QList<QByteArray> classNames = QList<QByteArray>(),
                 R_CFinalizer_t finalizer = NULL);
SEXP wrapSmoke(void *ptr, const char *className, bool allocated = true);

#endif

