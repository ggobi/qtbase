/*
 *  Inspired by the Rttpd.c file from R library and a discussion with
 *  Simon Urbanek.  Kaiser Md. Nahiduzzaman added the Windows support,
 *  which is still unstable, perhaps because Qt is running another
 *  instance of the event loop in a separate thread. Running ours at
 *  least catches enough events to ensure that most event handling is
 *  synchronized with R.
 */

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
#include <R_ext/eventloop.h> /* UNIX-specific input handler implementation */
#include <unistd.h>
#else
/* --- Windows-only --- */
#include <QMutexLocker>
extern __declspec(dllimport) uintptr_t R_CStackLimit; /* C stack limit */
extern __declspec(dllimport) uintptr_t R_CStackStart; /* Initial stack address */
#include <windows.h>	  
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define WM_EVENTLOOP_CALLBACK ( WM_USER + 1 )
static HWND message_window;
static LRESULT CALLBACK
EventLoopWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#ifndef HWND_MESSAGE
#define HWND_MESSAGE ((HWND)-3) /* NOTE: this is supported by >=W2k/XP only! */
#endif

static int in_process = 0;

QMutex mutex;

/* --- flag determining whether one-time initialization is yet to be
       performed --- */
static int needs_init = 1;

static void callback_input_handler();
static void QEventLoop_exec();

#endif/* WIN32 */


/* Much of this code inspired by Simon Urbanek's CarbonEL package */
/* The Windows platform specific code is inspired by Rhttpd.c file in
   R -- Kaiser */

QApplication *app;
static int qapp_argc = 2;
static char *qapp_argv[] = { "qtbase", "-nograb" };

static int processingEvent = 0;
static int fired = 0, active = 1;
static QtMessageHandler prevMsgHandler;

#ifndef WIN32
InputHandler *eventLoopInputHandler = NULL;
static int ifd, ofd;
//static int fired = 0, active = 1;//moved outside of ifndef WIN32

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

#else
/* WIN32 */
void EventLoop::run() {						   
  //Do busy polling in Windows too
  while(active) {
    if (!fired) {
      fired=1;
      callback_input_handler();
    }	
  }				  
  //Will never reach here
  return;
}

static void QEventLoop_exec()
{				 
    //An attempt to make the process thread-safe
  mutex.lock();
  try {
    //300 is an empirical value
    app->processEvents(QEventLoop::WaitForMoreEvents |
                       QEventLoop::EventLoopExec, 300);
    app->sendPostedEvents(0, 0);
  }
  catch(...) {
    Rprintf("Exception from processEvents | sendPostedEvents\n");
  }
  mutex.unlock();
}
#endif

static EventLoop* eventLoop = NULL;

void R_Qt_msgHandler(QtMsgType type, const QMessageLogContext &context,
                     const QString &msg)
{
  const char *charMsg = msg.toLocal8Bit().constData();
  switch (type) {
  case QtDebugMsg:
    Rprintf("Debug: %s\n", charMsg);
    break;
  case QtWarningMsg:
    warning(charMsg);
    break;
  case QtCriticalMsg:
    error(charMsg);
    break;
  case QtFatalMsg:
    error("FATAL: %s\n", charMsg);
  }
}

#ifdef WIN32
/* on Windows we have to guarantee that run_callback is performed
   on the main thread, so we have to dispatch it through a message */
//static void run_callback_main_thread(bg_conn_t *c);
static void run_callback_main_thread();

static void run_callback()
{
  /* SendMessage is synchronous, so it will wait until the message
     is processed */
  SendMessage(message_window, WM_EVENTLOOP_CALLBACK, 0, 0);
  // Alternative
  /* PostMessage is asynchronous, so it will return immediately */
  //PostMessage(message_window, WM_EVENTLOOP_CALLBACK, 0, (LPARAM) 0);
}
#define run_callback run_callback_main_thread

/* wrap the actual call with ToplevelExec since we need to have a guaranteed
   return so we can track the presence of a worker code inside R to prevent
   re-entrance from other clients */
static void run_callback()
{
  if (!in_process){
    in_process = 1;	
    if (!processingEvent) {
      processingEvent = 1;
      QEventLoop_exec();
      processingEvent = 0;
    }
    in_process = 0;
    fired=0;
  }	
}

#undef run_callback

static void first_init()
{
  /* create a dummy message-only window for synchronization with the
   * main event loop */
  HINSTANCE instance = GetModuleHandle(NULL);
  LPCTSTR str_class = "EventLoop";
  WNDCLASS wndclass = { 0, EventLoopWindowProc, 0, 0, instance, NULL, 0, 0,
                        NULL, str_class };
  RegisterClass(&wndclass);
  message_window = CreateWindow(str_class, "EventLoop", 0, 1, 1, 1, 1,
                                HWND_MESSAGE, NULL, instance, NULL);
  needs_init = 0;
}									
#endif/* WIN32 */

static void 
R_Qt_init()
{
  prevMsgHandler = qInstallMessageHandler(R_Qt_msgHandler);
  app = new QApplication(qapp_argc, qapp_argv);
  //following call starts a thread and will run the Qt event loop
  //there, which may never return -- Kaiser app->exec();
#ifdef WIN32
  /* WIN32 */
  if (needs_init) /* initialization may need to be performed on first use */
	first_init();  
#endif/* WIN32 */
}
								
static void 
R_Qt_cleanup()
{
#ifndef WIN32
  active = FALSE;
  eventLoop->wait();
  delete eventLoop;
  removeInputHandler(&R_InputHandlers, eventLoopInputHandler);
  close(ifd);
  close(ofd); 
#endif
  Rprintf("app->quit().");
  app->quit();
  qInstallMessageHandler(prevMsgHandler);
  delete app;
}


#ifdef WIN32
/* Windows implementation uses threads to do the same as watching the
   FD and the main event loop to synchronize with R through a
   message-only window which is created on the R thread */
static LRESULT CALLBACK EventLoopWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
                                            LPARAM lParam)
{
  if (hwnd == message_window && uMsg == WM_EVENTLOOP_CALLBACK) {
    run_callback_main_thread();
    return 0;
  }
  return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

/* this is really superfluous - we could just cast run_callback
   accordingly .. - Simon */
static void callback_input_handler()
{
  run_callback();
}
#endif

void EventLoop::begin() {
  if (!qApp) {
    R_Qt_init();

#ifndef WIN32
    int fds[2];
    
    /* Experimental timer-based piping to a file descriptor */
    if (!pipe(fds)) {
      ifd = fds[0];
      ofd = fds[1];
      eventLoopInputHandler = addInputHandler(R_InputHandlers, ifd,
                                              R_Qt_timerInputHandler, 32);
      R_CStackLimit = -1;

      eventLoop = new EventLoop();
      eventLoop->start();
    } else error("Failed to establish pipe for event handling");
#else
    /* WIN32 */
    /* do the desired Windows synchronization */
    /* disable stack checking, because threads will thow it off */
    R_CStackLimit = (uintptr_t) -1;

    //Rprintf("Entering eventloop thread...");
    eventLoop = new EventLoop();
    eventLoop->start();
    //Rprintf("Outside eventloop thread...");	
#endif
  }
}							
									   
extern "C" {
  // Entry point in this file
  SEXP 
  addQtEventHandler()
  {
    //This is the function that gets called from R
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
