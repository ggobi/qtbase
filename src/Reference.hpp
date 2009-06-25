#include <QGraphicsWidget>

#include <QWidget>
#include <QHash>

namespace QViz {
/* For maintaining a reference count */
  class Reference : public QObject {
    Q_OBJECT
    
  private:
    // reference counted object
    void *_referee;
    // shared reference count hash for all pointers
    static QHash<void *, int> *counts;

    void clear();
    int count() { // returns the reference count
      return count(_referee);
    }
      
  public:
    // add a reference to referee's count
    Reference(void *referee, QObject *referer = NULL);
    // subtract from referee's count and release() if zero reached.
    // NOTE: every subclass must override with call to release(), if
    // deleteReferee is overridden, since destructors do not call
    // overrides in derived classes.
    virtual ~Reference() { };
    
    void *referee() { return _referee; }

    void invalidate();
    bool isValid() { return _referee != NULL; }

    static int count(void *referee);
    
  public slots:
    void destroy() {
      delete this;
    }
    
  protected:
    /* Subclass is responsible for actual deletion. We can't free a
       void *. We could if this was a template, for example, but I
       don't think signal/slot stuff would work. Of course, this means
       we need a specific subclass for every base class, but right
       now we only need this for QObject-derived types.
    */
    virtual void deleteReferee() = 0;
    void release(); // convenience to be called during destruction
  };
  
  class QObjectReference : public Reference {
    Q_OBJECT
  public:
    // hooks 'refereeDestroyed' to referee::destroyed
    QObjectReference(QObject *referee, QObject *referer = NULL);
    virtual ~QObjectReference();
    
  protected:
    // do not delete if has a parent
    virtual void deleteReferee();
                             
  protected slots:
    // invalidate
    virtual void refereeDestroyed();
  };

  class QWidgetReference : public QObjectReference {
    Q_OBJECT

    public:
    QWidgetReference(QWidget *referee, QObject *referer = NULL)
      : QObjectReference(referee, referer) { }
    virtual ~QWidgetReference();
    
    protected:
    virtual void deleteReferee();
  };

  class QGraphicsWidgetReference : public QObjectReference {
    Q_OBJECT

  public:
    QGraphicsWidgetReference(QGraphicsWidget *referee, QObject *referer = NULL)
      : QObjectReference(referee, referer) { }
    virtual ~QGraphicsWidgetReference();
    
  protected:
    virtual void deleteReferee();
  };


  class QGraphicsItemReference : public Reference {
    Q_OBJECT

    public:
    QGraphicsItemReference(QGraphicsItem *referee, QObject *referer = NULL)
      : Reference(referee, referer) { }
    virtual ~QGraphicsItemReference();
    
  protected:
    virtual void deleteReferee();
  };

}

extern "C" {
  void addQObjectReference(QObject *referee, QObject *referer);
  void addQWidgetReference(QWidget *referee, QObject *referer);
  void addQGraphicsWidgetReference(QGraphicsWidget *referee, QObject *referer);
  void addQGraphicsItemReference(QGraphicsItem *referee, QObject *referer);
}
