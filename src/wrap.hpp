#ifndef WRAPPERS_H
#define WRAPPERS_H

#include <QObject>
#include <QWidget>
#include <QGraphicsItem>

#include <R.h>
#include <Rinternals.h>

QObject *unwrapQObjectReferee(SEXP x);
QGraphicsItem *unwrapQGraphicsItemReferee(SEXP x);

#define unwrapPointerSep(x, rtype, ctype) ({                            \
      if (TYPEOF(x) != EXTPTRSXP)                                       \
        error("unwrapPointer: not an externalptr");                     \
      if (!inherits(x, #rtype))                                         \
        error("unwrapPointer: expected object of class '" # rtype "'"); \
      reinterpret_cast<ctype *>(R_ExternalPtrAddr(x));                  \
    })

#define unwrapPointer(x, type) unwrapPointerSep(x, type, type)

#define checkReference(x, type) ({                                      \
      if (!x || !x->isValid())                                          \
        error("checkReference: invalid reference to '" #type "'");      \
    })

#define unwrapReferenceSep(x, rtype, ctype) ({                          \
      ctype *ans =                                                      \
        qobject_cast<ctype *>(unwrapPointerSep(x, rtype, QObject));     \
      checkReference(ans, rtype);                                       \
      ans;                                                              \
    })

#define unwrapReference(x, type) ({                                     \
      type *ans =                                                       \
        qobject_cast<type *>(unwrapPointer(x, QObject));                \
      checkReference(ans, type);                                        \
      ans;                                                              \
    })

#define unwrapQObject(x, type) ({                                       \
      type *ans = qobject_cast<type *>(unwrapQObjectReferee(x));	\
      if (!ans) error("unwrapQObject: Coercion to " #type " failed");	\
      ans;								\
    })

#define unwrapQGraphicsItem(x, type) ({                                      \
      type *ans = qgraphicsitem_cast<type *>(unwrapQGraphicsItemReferee(x)); \
      if (!ans) error("unwrapQGraphicsItem: Coercion to " #type " failed");  \
      ans;								     \
    })

#define unwrapQGraphicsWidget(x) unwrapQGraphicsItem(x, QGraphicsWidget)
#define unwrapQWidget(x) unwrapQObject(x, QWidget)

extern "C" {
  SEXP wrapQObject(QObject *object);
  SEXP wrapQWidget(QWidget *widget);
  SEXP wrapPointer(void *ptr, const char * const *className,
                   R_CFinalizer_t finalizer);
  SEXP wrapQGraphicsItem(QGraphicsItem *item);
  SEXP wrapQGraphicsWidget(QGraphicsWidget *widget);
}

#endif

