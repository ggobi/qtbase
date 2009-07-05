#include <QVariant>
#include <QWidget>
#include <QGraphicsWidget>

#include "Reference.hpp"
#include "wrap.hpp"
#include <Rdefines.h>

extern "C" SEXP asRStringArray(const char * const * strs);

QObject *unwrapQObjectReferee(SEXP x) {
  QObject *ptr = unwrapPointer(x, QObject);
  Reference *ref = qobject_cast<QObjectReference *>(ptr);
  QObject *ans = NULL;
  if (ref && ref->isValid())
    ans = reinterpret_cast<QObject *>(ref->referee()); 
  else { // Special case for QGraphicsWidget
    Reference *iref = qobject_cast<QGraphicsItemReference*>(ptr);
    if (iref && iref->isValid()) {
      QGraphicsItem *item = reinterpret_cast<QGraphicsItem *>(iref->referee());
      ans = qgraphicsitem_cast<QGraphicsWidget *>(item);
    }
  }
  return ans;
}

QGraphicsItem *unwrapQGraphicsItemReferee(SEXP x) {
  QGraphicsItemReference *ref =
    unwrapReferenceSep(x, QGraphicsItem, QGraphicsItemReference);
  return reinterpret_cast<QGraphicsItem *>(ref->referee());
}

extern "C" {
  
  void finalizeReference(SEXP extp) {
    Reference *r = unwrapReference(extp, Reference);
    delete r;
  }

  SEXP wrapReference(Reference *ref, const char * const * classNames = NULL) {
    SEXP ans = wrapPointer(ref, classNames, finalizeReference);
    return ans;
  }

  static SEXP getQObjectClasses(QObject *obj,
                                const char * const * extraClasses = NULL)
  {
    const QMetaObject *meta = obj->metaObject(), *m;
    int nclasses = 0;
    SEXP classes, rextras;
    if (extraClasses) {
      PROTECT(rextras = asRStringArray(extraClasses));
      nclasses = length(rextras);
    }
    for (m = meta; m; m = m->superClass())
      nclasses++;
    PROTECT(classes = allocVector(STRSXP, nclasses));
    int i = 0;
    for (m = meta; m; m = m->superClass())
      SET_STRING_ELT(classes, i++, mkChar(m->className()));
    if (extraClasses) {
      for(int j = 0; i < nclasses; i++, j++)
        SET_STRING_ELT(classes, i, STRING_ELT(rextras, j));
      UNPROTECT(1);
    }
    UNPROTECT(1);
    return classes;
  }
  
  SEXP wrapQObjectReference(QObjectReference *ref) {
    SEXP ans;
    PROTECT(ans = wrapReference(ref));
    SET_CLASS(ans, getQObjectClasses((QObject *)ref->referee()));
    UNPROTECT(1);
    return ans;
  }
  
  SEXP wrapQWidget(QWidget *widget) {
    return wrapQObjectReference(new QWidgetReference(widget));
  }
  SEXP wrapQObject(QObject *object) {
    return wrapQObjectReference(new QObjectReference(object));
  }
  SEXP wrapQGraphicsItem(QGraphicsItem *item) {
    const char *classes[] = { "QGraphicsItem", NULL };
    return wrapReference(new QGraphicsItemReference(item), classes);
  }
  SEXP wrapQGraphicsWidget(QGraphicsWidget *widget) {
    const char *classes[] = { "QGraphicsItem", "QGraphicsLayoutItem", NULL };
    SEXP ans;
    Reference *ref = new QGraphicsItemReference(widget);
    PROTECT(ans = wrapReference(ref, NULL));
    SET_CLASS(ans, getQObjectClasses(widget, classes));
    UNPROTECT(1);
    return ans;
  }
  
  SEXP wrapPointer(void *ptr, const char * const * classNames,
                   R_CFinalizer_t finalizer)
  {
    SEXP ans = R_MakeExternalPtr(ptr, R_NilValue, R_NilValue);
    if (finalizer)
      R_RegisterCFinalizer(ans, finalizer);
    if (classNames)
      SET_CLASS(ans, asRStringArray(classNames));
    return ans;
  }
}


// QWidget *unwrapToQWidget(SEXP x) {
//     return unwrapQObject(x, QWidget);
// }

