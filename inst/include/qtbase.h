#ifndef QTBASE_H
#define QTBASE_H

// FIXME: we should not have duplicate header files inside and outside
// the package. Could use symbolic links.

#include <QList>
#include <QString>

#include <Rinternals.h>

#include <qtdefs.h>

class Smoke;

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

#define unwrapSmoke(x, type) reinterpret_cast<type *>(_unwrapSmoke(x, #type))

QT_BEGIN_DECLS

void *_unwrapSmoke(SEXP x, const char *type);

SEXP wrapPointer(void *ptr, QList<QByteArray> classNames = QList<QByteArray>(),
                 R_CFinalizer_t finalizer = NULL);

SEXP _wrapSmoke(void *ptr, const char *className, bool allocated = true);
#define wrapSmoke(ptr, type, allocated) _wrapSmoke(ptr, #type, allocated)
#define wrapSmokeCopy(ptr, type) _wrapSmoke(new type(ptr), #type, true)

// Invoke a Smoke method with R types
SEXP invokeSmokeMethod(Smoke *smoke, short index, SEXP x, SEXP args);

// Conversion routines

QString sexp2qstring(SEXP s);
SEXP qstring2sexp(QString s);

// Smoke module registration
Smoke *registerSmokeModule(Smoke *smoke);

QT_END_DECLS


#endif
