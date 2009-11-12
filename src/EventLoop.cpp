#include <QApplication>
#include <QTimer>
#include "EventLoop.hpp"

#include <Rinternals.h>

#ifndef WIN32
/* We need these symbols, but <Rinterface.h> declares them using C99
   uintptr_t, which no longer works with gcc 4.4. It seems like
   quintptr is more or less an alias. Could probably check something
   at build time, but without autconf, it's annoying. Don't tell BR. */
extern quintptr R_CStackLimit;	/* C stack limit */
extern quintptr R_CStackStart;	/* Initial stack address */
#include <R_ext/eventloop.h>
#include <unistd.h>
#endif


/* Much of this code inspired by Simon Urbanek's CarbonEL package */

QApplication *app;
static int qapp_argc = 1;
static char *qapp_argv[] = { "qtbase" };

static int processingEvent = 0;

#ifndef WIN32
static int ifd, ofd;
static int fired = 0, active = 1;

static void 
R_Qt_eventHandler()
{
  if (!processingEvent) {
    processingEvent = 1;
    app->processEvents();
    processingEvent = 0;
  }
}

static void
R_Qt_timerInputHandler(void *data) {
  Q_UNUSED(data);
  char buf[16];
  read(ifd, buf, 16);
  R_Qt_eventHandler();  
  fired=0;
}

void EventLoop::run() {
  char buf[16];
  while(active) {
    msleep(10);
    if (!fired) {
      fired=1; *buf=0;
      write(ofd, buf, 1);
    }
  }
}

static EventLoop* eventLoop = NULL;

#else

void EventLoop::run() {
  return;
}

#endif

static void 
R_Qt_init()
{
  app = new QApplication(qapp_argc, qapp_argv);
  // app->exec();
}

static void 
R_Qt_cleanup()
{
  delete app;
  #ifndef WIN32
  active = FALSE;
  delete eventLoop;
  #endif
}

void EventLoop::begin() {
  if (!qApp) {
    R_Qt_init();

#ifndef WIN32
    int fds[2];
    
    /* Experimental timer-based piping to a file descriptor */
    if (!pipe(fds)) {
      ifd = fds[0];
      ofd = fds[1];
      addInputHandler(R_InputHandlers, ifd, R_Qt_timerInputHandler, 32);
      R_CStackLimit = -1;

      eventLoop = new EventLoop();
      eventLoop->start();
    } else error("Failed to establish pipe for event handling");
    
#endif
  }
}

extern "C" {
  SEXP 
  addQtEventHandler()
  {
    EventLoop::begin();
    return R_NilValue;
  }

  SEXP 
  cleanupQtApp()
  {
    R_Qt_cleanup();
    return R_NilValue;
  }
}
