#ifndef WRAPPERS_H
#define WRAPPERS_H

#include <QObject>
#include <QWidget>
#include <QGraphicsItem>
#include <QGraphicsLayoutItem>

#include <R.h>
#include <Rinternals.h>

/* NOTE: everything except unwrapPointer and unwrapSmoke is deprecated */

QObject *unwrapQObjectReferee(SEXP x);
QGraphicsItem *unwrapQGraphicsItemReferee(SEXP x);
QGraphicsLayoutItem *unwrapQGraphicsLayoutItemReferee(SEXP x);

#define unwrapPointerSep(x, rtype, ctype) ({                            \
      if (TYPEOF(x) != EXTPTRSXP)                                       \
        error("unwrapPointer: not an externalptr");                     \
      if (!inherits(x, #rtype))                                         \
        error("unwrapPointer: expected object of class '" #rtype "'");  \
      reinterpret_cast<ctype *>(R_ExternalPtrAddr(x));                  \
    })

#define unwrapPointer(x, type) unwrapPointerSep(x, type, type)

void *_unwrapSmoke(SEXP x, const char *type);

#define unwrapSmoke(x, type) reinterpret_cast<type *>(_unwrapSmoke(x, #type))

#define unwrapReference(x, type) ({                                     \
      type *ans =                                                       \
        qobject_cast<type *>(unwrapPointer(x, type));                   \
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

#define unwrapQGraphicsLayoutItem(x, type) \
  reinterpret_cast<type *>(unwrapQGraphicsLayoutItemReferee(x))

#define unwrapQGraphicsWidget(x) unwrapQGraphicsItem(x, QGraphicsWidget)
#define unwrapQWidget(x) unwrapQObject(x, QWidget)

extern "C" {
  SEXP wrapQObject(QObject *object);
  SEXP wrapQWidget(QWidget *widget);
  SEXP wrapPointer(void *ptr,
                   QList<QByteArray> classNames = QList<QByteArray>(),
                   R_CFinalizer_t finalizer = NULL);
  SEXP wrapQGraphicsItem(QGraphicsItem *item,
                         QList<QByteArray> classNames = QList<QByteArray>());
  SEXP wrapQGraphicsWidget(QGraphicsWidget *widget);
  SEXP wrapQGraphicsLayoutItem(QGraphicsLayoutItem *item,
                               QList<QByteArray> classes);
}

#endif

