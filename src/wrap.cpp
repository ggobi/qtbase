#include <QVariant>
#include <QWidget>
#include <QGraphicsWidget>

#include "Reference.hpp"
#include "wrap.hpp"
#include <Rdefines.h>

extern "C" SEXP qstring2sexp(QString str);

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
  QGraphicsItemReference *ref = unwrapReference(x, QGraphicsItemReference);
  return reinterpret_cast<QGraphicsItem *>(ref->referee());
}

extern "C" {
  
  void finalizeReference(SEXP extp) {
    Reference *r = unwrapReference(extp, Reference);
    delete r;
  }

  SEXP wrapReference(Reference *ref, QList<QString> classes) {
    classes.append("Reference");
    SEXP ans = wrapPointer(ref, classes, finalizeReference);
    return ans;
  }

  static QList<QString>
  getQObjectClasses(QObject *obj)
  {
    const QMetaObject *meta = obj->metaObject(), *m;
    //int nclasses = 0;
    QList<QString> classes;
    /*
    SEXP classes, rextras;
    if (extraClasses) {
      PROTECT(rextras = asRStringArray(extraClasses));
      nclasses = length(rextras);
    }
    for (m = meta; m; m = m->superClass())
      nclasses++;
    PROTECT(classes = allocVector(STRSXP, nclasses));
    */
    //int i = 0;
    for (m = meta; m; m = m->superClass())
      //SET_STRING_ELT(classes, i++, mkChar(m->className()));
      classes.append(m->className());
    /*
    if (extraClasses) {
      for(int j = 0; i < nclasses; i++, j++)
        SET_STRING_ELT(classes, i, STRING_ELT(rextras, j));
        UNPROTECT(1);
    }
    */
    //UNPROTECT(1);
    return classes;
  }
  
  SEXP wrapQObjectReference(QObjectReference *ref) {
    return wrapReference(ref, getQObjectClasses((QObject *)ref->referee()));
  }
  
  SEXP wrapQWidget(QWidget *widget) {
    return wrapQObjectReference(new QWidgetReference(widget));
  }
  SEXP wrapQObject(QObject *object) {
    return wrapQObjectReference(new QObjectReference(object));
  }
  SEXP wrapQGraphicsItemReference(QGraphicsItemReference *ref,
                                  QList<QString> classes)
  {
    classes.append("QGraphicsItemReference");
    return wrapReference(ref, classes);
  }
  SEXP wrapQGraphicsItem(QGraphicsItem *item, QList<QString> classes)
  {
    classes.append("QGraphicsItem");
    return wrapQGraphicsItemReference(new QGraphicsItemReference(item),
                                      classes);
  }
  SEXP wrapQGraphicsWidget(QGraphicsWidget *widget) {
    return wrapQGraphicsItem(widget, getQObjectClasses(widget));
  }
  
  SEXP wrapPointer(void *ptr, QList<QString> classNames,
                   R_CFinalizer_t finalizer)
  {
    SEXP ans;
    PROTECT(ans = R_MakeExternalPtr(ptr, R_NilValue, R_NilValue));
    if (finalizer)
      R_RegisterCFinalizer(ans, finalizer);
    SEXP rclassNames = allocVector(STRSXP, classNames.size());
    SET_CLASS(ans, rclassNames);
    for (int i = 0; i < length(rclassNames); i++)
      SET_STRING_ELT(rclassNames, i,
                     mkChar(classNames[i].toLocal8Bit().data()));
    UNPROTECT(1);
    return ans;
  }
}


// QWidget *unwrapToQWidget(SEXP x) {
//     return unwrapQObject(x, QWidget);
// }

