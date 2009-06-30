#ifndef WRAPPERS_H
#define WRAPPERS_H

#include <QObject>
#include <QWidget>
#include <QGraphicsItem>

#include <R.h>
#include <Rinternals.h>

QObject *unwrapQObjectReferee(SEXP x);
QGraphicsItem *unwrapQGraphicsItemReferee(SEXP x);

#define unwrapPointer(x, type) ({                        \
      if (TYPEOF(x) != EXTPTRSXP)                       \
        error("unwrapPointer: not an externalptr");      \
      reinterpret_cast<type *>(R_ExternalPtrAddr(x));   \
})

#define unwrapReference(x, type) ({                                     \
      type *ans = qobject_cast<type *>(unwrapPointer(x, QObject));      \
      if (!ans || !ans->isValid())                                      \
        error("unwrapReference: Coercion to '" #type "' failed");       \
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

