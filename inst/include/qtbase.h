#ifndef QTBASE_H
#define QTBASE_H

#include <R.h>
#include <Rinternals.h>
#include <QObject>
#include <QWidget>
#include <QString>

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

extern "C" {
  SEXP wrapQObject(QObject *object);
  SEXP wrapQWidget(QWidget *widget);
  SEXP wrapPointer(void *ptr, const char *className, R_CFinalizer_t finalizer);
}



QString sexp2qstring(SEXP s);
extern "C" {
SEXP qstring2sexp(QString s);
}



#endif
