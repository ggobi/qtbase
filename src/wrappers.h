#ifndef WRAPPERS_H
#define WRAPPERS_H

#include <R.h>
#include <Rinternals.h>
#include <QObject>
#include <QWidget>

QObject *unwrapQObjectReferee(SEXP x);

#define unwrapPointer(x, type) ((type *) R_ExternalPtrAddr(x))

#define unwrapReference(x, type) ({                                         \
      type *ans = qobject_cast<type *>(unwrapPointer(x, QObject));          \
      if (!ans) error("unwrapReference: Coercion to '" #type "' failed");   \
      ans;                                                                  \
    })


/* QViz::QObjectReference *ref = unwrapReference(x, QViz::QObjectReference); \ */
/* type *ans = qobject_cast<type *>((QObject *)ref->referee());		\ */

#define unwrapQObject(x, type) ({                                              \
      type *ans = qobject_cast<type *>(unwrapQObjectReferee(x));	\
      if (!ans) error("unwrapQObject: Coercion to " #type " failed");	\
      ans;								\
    })

extern "C" {
  SEXP wrapQObject(QObject *object);
  SEXP wrapQWidget(QWidget *widget);
  SEXP wrapPointer(void *ptr, const char *className, R_CFinalizer_t finalizer);
}


/* QWidget *unwrapToQWidget(SEXP x); */

/* #define unwrapQWidget(x, type) ({                                              	\ */
/*       type *ans = qobject_cast<type *>(unwrapQObject2QWidget(x));		\ */
/*       if (!ans) error("unwrapQObject: Coercion to " #type " failed");		\ */
/*       ans;                                                       		\ */
/*     }) */


#endif

