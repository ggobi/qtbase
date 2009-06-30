#ifndef QTBASE_H
#define QTBASE_H

// FIXME: we should not have duplicate header files inside and outside
// the package. Could use symbolic links.

#include <QObject>
#include <QWidget>
#include <QString>
#include <QGraphicsItem>

#include <Rinternals.h>

#include <qtdefs.h>

QObject *unwrapQObjectReferee(SEXP x);
QGraphicsItem *unwrapQGraphicsItemReferee(SEXP x);

#define unwrapPointer(x, type) ({                        \
      if (TYPEOF(x) != EXTPTRSXP)                       \
        error("unwrapPointer: not an externalptr");      \
      reinterpret_cast<type *>(R_ExternalPtrAddr(x));   \
    })

#define unwrapReference(x, type) ({                                         \
      type *ans = qobject_cast<type *>(unwrapPointer(x, QObject));          \
      if (!ans) error("unwrapReference: Coercion to '" #type "' failed");   \
      ans;                                                                  \
    })

#define unwrapQObject(x, type) ({                                              \
      type *ans = qobject_cast<type *>(unwrapQObjectReferee(x));	\
      if (!ans) error("unwrapQObject: Coercion to " #type " failed");	\
      ans;								\
    })

#define unwrapQGraphicsItem(x, type) ({                                      \
      type *ans = qgraphicsitem_cast<type *>(unwrapQGraphicsItemReferee(x)); \
      if (!ans) error("unwrapQGraphicsItem: Coercion to " #type " failed"); \
      ans;                                                              \
    })

#define unwrapQGraphicsWidget(x) unwrapQGraphicsItem(x, QGraphicsWidget)
#define unwrapQWidget(x) unwrapQObject(x, QWidget)

QT_BEGIN_DECLS

SEXP wrapQObject(QObject *object);
SEXP wrapQWidget(QWidget *widget);
SEXP wrapPointer(void *ptr, const char * const * classNames,
                 R_CFinalizer_t finalizer);
SEXP wrapQGraphicsItem(QGraphicsItem *item);
SEXP wrapQGraphicsWidget(QGraphicsWidget *widget);

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

// Reference counting
void addQObjectReference(QObject *referee, QObject *referer);
void addQWidgetReference(QWidget *referee, QObject *referer);
void addQGraphicsItemReference(QGraphicsItem *referee, QObject *referer);

QT_END_DECLS


#endif
