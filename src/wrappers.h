#ifndef WRAPPERS_H
#define WRAPPERS_H

#include <QObject>
#include <QWidget>
#include <QGraphicsWidget>

#include <R.h>
#include <Rinternals.h>

QObject *unwrapQObjectReferee(SEXP x);

#define unwrapPointer(x, type) ((type *) R_ExternalPtrAddr(x))

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

#define unwrapQWidget(x) unwrapQObject(x, QWidget)

extern "C" {
  SEXP wrapQObject(QObject *object);
  SEXP wrapQWidget(QWidget *widget);
  SEXP wrapPointer(void *ptr, const char *className, R_CFinalizer_t finalizer);
  SEXP wrapQGraphicsWidget(QGraphicsWidget *widget);
  SEXP wrapQGraphicsItem(QGraphicsItem *item);
}

#endif

