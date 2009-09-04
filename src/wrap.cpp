#include "SmokeObject.hpp"

#include <QVariant>
#include <QWidget>
#include <QGraphicsWidget>

#include "Reference.hpp"
#include "wrap.hpp"
#include <Rdefines.h>

void *_unwrapSmoke(SEXP x, const char *type) {
  void *ans = NULL;
  SmokeObject *so = unwrapPointer(x, SmokeObject);
  if (so)
    ans = so->castPtr(type);
  return ans;
}

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

QGraphicsLayoutItem *unwrapQGraphicsLayoutItemReferee(SEXP x) {
  QObject *ptr = unwrapPointer(x, QObject);
  Reference *ref = qobject_cast<QGraphicsLayoutItemReference *>(ptr);
  QGraphicsLayoutItem *ans = NULL;
  if (ref && ref->isValid())
    ans = reinterpret_cast<QGraphicsLayoutItem *>(ref->referee()); 
  else { // Special case for QGraphicsWidget
    Reference *iref = qobject_cast<QGraphicsItemReference*>(ptr);
    if (iref && iref->isValid()) {
      QGraphicsItem *item = reinterpret_cast<QGraphicsItem *>(iref->referee());
      ans = qgraphicsitem_cast<QGraphicsWidget *>(item);
    }
  }
  return ans;
}

extern "C" {
  
  void finalizeReference(SEXP extp) {
    Reference *r = unwrapReference(extp, Reference);
    delete r;
  }

  SEXP wrapReference(Reference *ref, QList<QByteArray> classes) {
    classes.append("Reference");
    SEXP ans = wrapPointer(ref, classes, finalizeReference);
    return ans;
  }

  static QList<QByteArray>
  getQObjectClasses(QObject *obj)
  {
    const QMetaObject *meta = obj->metaObject(), *m;
    QList<QByteArray> classes;
    for (m = meta; m; m = m->superClass())
      classes.append(m->className());
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
                                  QList<QByteArray> classes)
  {
    classes.append("QGraphicsItemReference");
    return wrapReference(ref, classes);
  }
  SEXP wrapQGraphicsItem(QGraphicsItem *item, QList<QByteArray> classes)
  {
    classes.append("QGraphicsItem");
    return wrapQGraphicsItemReference(new QGraphicsItemReference(item),
                                      classes);
  }
  SEXP wrapQGraphicsWidget(QGraphicsWidget *widget) {
    return wrapQGraphicsItem(widget, getQObjectClasses(widget));
  }
  SEXP wrapQGraphicsLayoutItemReference(QGraphicsLayoutItemReference *ref,
                                        QList<QByteArray> classes)
  {
    classes.append("QGraphicsLayoutItemReference");
    return wrapReference(ref, classes);
  }
  SEXP wrapQGraphicsLayoutItem(QGraphicsLayoutItem *item,
                               QList<QByteArray> classes)
  {
    if (!item->ownedByLayout())
      error("Layout item does not follow layout ownership");
    QGraphicsLayoutItemReference *ref = new QGraphicsLayoutItemReference(item);
    classes.append("QGraphicsLayoutItem");
    return wrapQGraphicsLayoutItemReference(ref, classes);
  }
  
  SEXP wrapPointer(void *ptr, QList<QByteArray> classNames,
                   R_CFinalizer_t finalizer)
  {
    SEXP ans;
    PROTECT(ans = R_MakeExternalPtr(ptr, R_NilValue, R_NilValue));
    if (finalizer)
      R_RegisterCFinalizer(ans, finalizer);
    SEXP rclassNames = allocVector(STRSXP, classNames.size());
    SET_CLASS(ans, rclassNames);
    for (int i = 0; i < length(rclassNames); i++)
      SET_STRING_ELT(rclassNames, i, mkChar(classNames[i].constData()));
    UNPROTECT(1);
    return ans;
  }
}
