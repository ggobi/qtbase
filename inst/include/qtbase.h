#ifndef QTBASE_H
#define QTBASE_H

#include <QObject>
#include <QWidget>
#include <QString>
#include <QGraphicsWidget>

#include <Rinternals.h>

QObject *unwrapQObjectReferee(SEXP x);

#define unwrapPointer(x, type) ((type *) R_ExternalPtrAddr(x))

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

// FIXME: we should not have duplicate header files inside and outside
// the package. Could use symbolic links.
extern "C" {
  SEXP wrapQObject(QObject *object);
  SEXP wrapQWidget(QWidget *widget);
  SEXP wrapPointer(void *ptr, const char *className, R_CFinalizer_t finalizer);
  SEXP wrapQGraphicsWidget(QGraphicsWidget *widget);
  SEXP wrapQGraphicsItem(QGraphicsItem *item);

  // Conversion routines
  // R -> C/Qt
  const char ** asStringArray(SEXP s_strs);
  QString sexp2qstring(SEXP s);
  // C -> R
  SEXP asRStringArray(const char * const * strs);
  SEXP qstring2sexp(QString s);

  // Reference counting
  void addQObjectReference(QObject *referee, QObject *referer);
  void addQWidgetReference(QWidget *referee, QObject *referer);
  void addQGraphicsWidgetReference(QGraphicsWidget *referee, QObject *referer);
  void addQGraphicsItemReference(QGraphicsItem *referee, QObject *referer);
}


#endif
