#include <QThread>

/* Singleton event loop -- start with begin() */
class EventLoop : public QThread {
  Q_OBJECT
private:
  EventLoop() { }
public:
  static void begin();
protected:
  virtual void run();
};
