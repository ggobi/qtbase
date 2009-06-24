
#include <QApplication>

#include "wrappers.h"
#include "utils.hpp"
#include "Reference.hpp"

#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>
#include <R_ext/Print.h>

#ifndef WIN32
#define CSTACK_DEFNS
#define HAVE_UINTPTR_T
#include <Rinterface.h>
#include <R_ext/eventloop.h>
#include <unistd.h>
#include <pthread.h>
#endif

extern "C" {
  
    // Test.cpp
    SEXP newLabelWidget(SEXP label);

    // style.cpp

    SEXP qt_qsetStyleSheet(SEXP x, SEXP s);
    SEXP qt_qstyleSheet(SEXP x);

    // actions.cpp

    SEXP qt_qaction(SEXP desc, SEXP shortcut, SEXP parent, SEXP tooltip,
                    SEXP checkable);
    SEXP qt_qaddActionToQWidget(SEXP w, SEXP a);
    SEXP qt_qaddAction_QGraphicsWidget(SEXP w, SEXP a);
    SEXP qt_qaddActionToQMenu(SEXP w, SEXP a);
    SEXP qt_qsetContextMenuPolicy(SEXP x, SEXP policy);

    // attributes.cpp

    SEXP qt_qsetDeleteOnClose(SEXP x, SEXP status);

    // this file 

    SEXP addQtEventHandler();
    SEXP cleanupQtApp();

    void R_init_qtbase(DllInfo *dll);

    extern void init_utils();

}

QObject* unwrapQObjectReferee(SEXP x);

QApplication *app;
int qapp_argc = 1;
char *qapp_argv[] = { "qtbase" };

static int processingEvent = 0;

void 
R_Qt_init()
{
    app = new QApplication(qapp_argc, qapp_argv);
    // app->exec();
}

void 
R_Qt_eventHandler()
{
    if (!processingEvent) {
	processingEvent = 1;
	app->processEvents();
	processingEvent = 0;
    }
    // app->processEvents(QEventLoop::DeferredDeletion, 100);
}

static void 
R_Qt_cleanup()
{
    delete app;
}

#define CALLDEF(name, n)  {#name, (DL_FUNC) &name, n}

static R_CallMethodDef CallEntries[] = {



    CALLDEF(qt_qsetStyleSheet, 2),
    CALLDEF(qt_qstyleSheet, 1),

    CALLDEF(addQtEventHandler, 0),
    CALLDEF(cleanupQtApp, 0),

    CALLDEF(newLabelWidget, 1),

    CALLDEF(qt_qaction, 5),
    CALLDEF(qt_qaddActionToQWidget, 2),
    CALLDEF(qt_qaddActionToQMenu, 2),
    CALLDEF(qt_qaddAction_QGraphicsWidget, 2),
    CALLDEF(qt_qsetContextMenuPolicy, 2),

    CALLDEF(qt_qsetDeleteOnClose, 2),

    {NULL, NULL, 0}
};


#define REG_CALLABLE(fun) R_RegisterCCallable("qtbase", #fun, (DL_FUNC) fun)

void R_init_qtbase(DllInfo *dll)
{
    init_utils(); // initializes some things unrelated to event handling

    // Register C routines
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);

    // Register for calling by other packages.
    REG_CALLABLE(wrapQWidget);
    REG_CALLABLE(wrapQObject);
    REG_CALLABLE(wrapPointer);
    REG_CALLABLE(unwrapQObjectReferee);
    REG_CALLABLE(wrapQGraphicsWidget);
    
    REG_CALLABLE(asStringArray);
    REG_CALLABLE(sexp2qstring);
    REG_CALLABLE(asRStringArray);
    REG_CALLABLE(qstring2sexp);
    
    REG_CALLABLE(addQObjectReference);
    REG_CALLABLE(addQWidgetReference);
    REG_CALLABLE(addQGraphicsWidgetReference);
}

/* Much of this code inspired by Simon Urbanek's CarbonEL package */

#ifndef WIN32
static int ifd, ofd;
static int fired = 0, active = 1;

static unsigned long intersleep = 10;

static void
R_Qt_timerInputHandler(void *data) {
  char buf[16];
  read(ifd, buf, 16);
  R_Qt_eventHandler();  
  fired=0;
}

static void millisleep(unsigned long tout) {
  struct timeval tv;
  tv.tv_usec = (tout%1000)*1000;
  tv.tv_sec  = tout/1000;
  select(0, 0, 0, 0, &tv);
}

static void*
R_Qt_thread(void *data) {
  char buf[16];
  while (active) {
    millisleep(intersleep);
    if (!fired) {
      fired=1; *buf=0;
      write(ofd, buf, 1);
    }
  }
  return 0;
}
#endif
    
SEXP 
addQtEventHandler()
{
  // at most one qApp can be running at a time
  if (!qApp) {
    R_Qt_init();

#ifndef WIN32
    int fds[2];
    
    /* Experimental timer-based piping to a file descriptor */
    if (!pipe(fds)) {
      pthread_t t;
      pthread_attr_t ta;
          
      ifd = fds[0];
      ofd = fds[1];
      addInputHandler(R_InputHandlers, ifd, R_Qt_timerInputHandler, 32);
      R_CStackLimit = -1;

      pthread_attr_init(&ta);
      pthread_attr_setdetachstate(&ta, PTHREAD_CREATE_DETACHED);
      pthread_create(&t,&ta,R_Qt_thread,0);
    } else error("Failed to establish pipe for event handling");

    // R_PolledEvents = R_Qt_eventHandler;
    // if (R_wait_usec > 10000 || R_wait_usec == 0)
    //     R_wait_usec = 10000;

#endif

  }
  return R_NilValue;
}


SEXP 
cleanupQtApp()
{
    R_Qt_cleanup();
    return R_NilValue;
}








