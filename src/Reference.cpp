#include "Reference.hpp"

#include <R.h>
#include <Rinternals.h>

/* So far, we handle types derived from QObject and
   QGraphicsItem. Other types will need their own custom classes. It
   may be that defining a new Reference type for every new base class
   is too much work. We could wrap the referee in QVariant and do
   everything at run-time (but it would bring more overhead). Note that
   we would need to define many QMetaTypes ourselves. */

QHash<void *, int> * Reference::counts = NULL;

Reference::Reference(void *referee, QObject *referer) : _referee(referee) {
  if (!counts)
    counts = new QHash<void *, int>;
  counts->insert(referee, count() + 1);
  if (referer)
    connect(referer, SIGNAL(destroyed()), this, SLOT(destroy()));
}

void Reference::clear() {
  counts->insert(_referee, count() - 1);
}

void Reference::release() {
  if (isValid()) {
    //printf("releasing reference to %p\n", _referee);
    clear();
    if (!count()) {
      //printf("references depleted, deleting %p\n", _referee);
      deleteReferee();
    }
    _referee = NULL;
  }
}

void Reference::invalidate() {
  if (isValid()) {
    clear();
    _referee = NULL;
  }
}

/* static methods, for managing the hash */

int Reference::count(void *referee) {
  return counts->value(referee);
}

/* QObjectReference */

QObjectReference::QObjectReference(QObject *referee, QObject *referer)
  : Reference(referee, referer)
{
  connect(referee, SIGNAL(destroyed()), this, SLOT(refereeDestroyed()));
}

QObjectReference::~QObjectReference() {
  release();
}

void QObjectReference::deleteReferee() {
  if (!((QObject *)referee())->parent()) { // Qt does not own it, free
    //Rprintf("QObject is parentless, freeing <%p>\n", referee());
    delete (QObject *)referee();
  }
}

void QObjectReference::refereeDestroyed() {
  // printf("QObject referee destroyed, invalidating %p\n", referee());
  invalidate();
}

/* QWidgetReference */

QWidgetReference::~QWidgetReference() {
  release();
}

void QWidgetReference::deleteReferee() {
  QWidget *widget = qobject_cast<QWidget *>((QObject *)referee());
  if (!widget->parent() && widget->isVisible()) {
    //printf("widget still visible, preserving %p\n", referee());
    widget->setAttribute(Qt::WA_DeleteOnClose);
  } else QObjectReference::deleteReferee();
}

// QGraphicsItem

QGraphicsItemReference::~QGraphicsItemReference() {
  release();
}

void QGraphicsItemReference::deleteReferee() {
  QGraphicsItem *item = reinterpret_cast<QGraphicsItem *>(referee());
  if (!item->parentItem() && !item->scene()) {
    delete item;
  } 
  // else printf("graphics item has parent item, preserving %p\n", referee());
}

QGraphicsLayoutItemReference::~QGraphicsLayoutItemReference() {
  release();
}

void QGraphicsLayoutItemReference::deleteReferee() {
  QGraphicsLayoutItem *item = reinterpret_cast<QGraphicsLayoutItem*>(referee());
  if (!item->parentLayoutItem()) {
    delete item;
  }
  // else printf("graphics layout item has parent, preserving %p\n", referee());
}

extern "C" {
  void addQObjectReference(QObject *referee, QObject *referer) {
    new QObjectReference(referee, referer);
  }
  void addQWidgetReference(QWidget *referee, QObject *referer) {
    new QWidgetReference(referee, referer);
  }
  void addQGraphicsItemReference(QGraphicsItem *referee, QObject *referer) {
    new QGraphicsItemReference(referee, referer);
  }
  void addQGraphicsLayoutItemReference(QGraphicsLayoutItem *referee,
                                       QObject *referer)
  {
    new QGraphicsLayoutItemReference(referee, referer);
  }
}
