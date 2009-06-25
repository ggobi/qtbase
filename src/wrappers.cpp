#include <QVariant>
#include <QWidget>

#include "Reference.hpp"
#include "wrappers.h"
#include <Rdefines.h>

using namespace QViz;

QObject *unwrapQObjectReferee(SEXP x) {
    QObjectReference *ref = unwrapReference(x, QObjectReference);
    return (QObject *)ref->referee();
}

extern "C" {
  
  void finalizeReference(SEXP extp) {
    Reference *r = unwrapReference(extp, Reference);
    delete r;
  }

  SEXP wrapReference(Reference *ref) {
    SEXP ans = R_MakeExternalPtr(ref, R_NilValue, R_NilValue);
    R_RegisterCFinalizer(ans, finalizeReference);
    return ans;
  }
  
  SEXP wrapQObjectReference(QObjectReference *ref) {
    SEXP ans;
    PROTECT(ans = wrapReference(ref));
    
    const QMetaObject *meta = ((QObject *)ref->referee())->metaObject(), *m;
    int nclasses = 0;
    SEXP classes;
    for (m = meta; m; m = m->superClass())
      nclasses++;
    PROTECT(classes = allocVector(STRSXP, nclasses));
    int i = 0;
    for (m = meta; m; m = m->superClass())
      SET_STRING_ELT(classes, i++, mkChar(m->className()));
    SET_CLASS(ans, classes);
    
    UNPROTECT(2);
    return ans;
  }
  
  SEXP wrapQWidget(QWidget *widget) {
    return wrapQObjectReference(new QWidgetReference(widget));
  }
  SEXP wrapQObject(QObject *object) {
    return wrapQObjectReference(new QObjectReference(object));
  }
  SEXP wrapQGraphicsWidget(QGraphicsWidget *widget) {
    return wrapQObjectReference(new QGraphicsWidgetReference(widget));
  }
  SEXP wrapQGraphicsItem(QGraphicsItem *item) {
    return wrapReference(new QGraphicsItemReference(item));
  }
  
  SEXP wrapPointer(void *ptr, const char *className, R_CFinalizer_t finalizer)
  {
    SEXP ans = R_MakeExternalPtr(ptr, R_NilValue, R_NilValue);
    if (finalizer)
      R_RegisterCFinalizer(ans, finalizer);
    SET_CLASS(ans, mkString(className));
    return ans;
  }

}


// QWidget *unwrapToQWidget(SEXP x) {
//     return unwrapQObject(x, QWidget);
// }

