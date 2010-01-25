#ifndef QTBASE_H
#define QTBASE_H

// FIXME: we should not have duplicate header files inside and outside
// the package. Could use symbolic links.

#include <smoke.h>

#include <QString>
#include <QRectF>
#include <QSizeF>
#include <QPointF>
#include <QMatrix>
#include <QColor>
#include <QFontF>

#include <Rinternals.h>

#include <qtdefs.h>

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

SEXP wrapPointer(void *ptr, QList<QString> classNames = QList<QString>(),
                 R_CFinalizer_t finalizer = NULL);

// Conversion routines
// R -> C/Qt
const char ** asStringArray(SEXP s_strs);
QString sexp2qstring(SEXP s);

QRectF asQRectF(SEXP r);
QPointF asQPointF(SEXP p);
QSizeF asQSizeF(SEXP s);
QMatrix asQMatrix(SEXP m);

QColor *asQColors(SEXP c);
QColor asQColor(SEXP c);
QFont asQFont(SEXP f);

// C -> R
SEXP asRStringArray(const char * const * strs);
SEXP qstring2sexp(QString s);

SEXP asRRectF(QRectF rect);
SEXP asRMatrix(QMatrix matrix, bool inverted);
SEXP asRPointF(QPointF point);
SEXP asRSizeF(QSizeF size);

SEXP asRColor(QColor color);
SEXP asRFont(QFont font);

// Smoke module registration
void registerRQtModule(Smoke *smoke);

QT_END_DECLS


#endif
