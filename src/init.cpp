
#include <QApplication>

#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>
#include <R_ext/Print.h>

#ifndef WIN32
#include <R_ext/eventloop.h>
#endif

extern "C" {

    //  "wrappers.h"

    SEXP wrapQObject(QObject *object);
    SEXP wrapQWidget(QWidget *widget);

    // Test.cpp
    SEXP newLabelWidget(SEXP label);

    // qwidgetWrappers.cpp

    SEXP qt_qshow(SEXP x);
    SEXP qt_qupdate(SEXP x);
    SEXP qt_qclose(SEXP x);
    SEXP qt_qraise(SEXP x);
    SEXP qt_qlower(SEXP x);
    SEXP qt_qparent(SEXP x);
    SEXP qt_qsetParent(SEXP x, SEXP parent);
    SEXP qt_qresize(SEXP x, SEXP w, SEXP h);
    SEXP qt_qheight(SEXP x);
    SEXP qt_qwidth(SEXP x);
    SEXP qt_qsetMinimumSize(SEXP x, SEXP w, SEXP h);
    SEXP qt_setExpanding(SEXP x, SEXP vertical, SEXP horizontal);
    SEXP qt_qsetContentsMargins(SEXP x, SEXP left, SEXP top, SEXP right, SEXP bottom);
    SEXP qt_qisEnabled(SEXP x);
    SEXP qt_qsetEnabled(SEXP x, SEXP flag);
    SEXP qt_qwindowTitle(SEXP x);
    SEXP qt_qsetWindowTitle(SEXP x, SEXP title);
    SEXP qt_qrender(SEXP x, SEXP file);
    SEXP qt_qrenderGraphicsView(SEXP x);
    SEXP qt_qrenderToPixmap(SEXP x, SEXP file);
    SEXP qt_qrenderToSVG(SEXP x, SEXP file);
    SEXP qt_qsetStyleSheet(SEXP x, SEXP s);
    SEXP qt_qstyleSheet(SEXP x);

    // containers.cpp

    SEXP qt_qtabWidget();
    SEXP qt_qaddTab(SEXP x, SEXP tab, SEXP label, SEXP index);
    SEXP qt_qremoveTab(SEXP x, SEXP index);
    SEXP qt_qstackedWidget();
    SEXP qt_qaddWidgetToStack(SEXP x, SEXP w, SEXP index);
    SEXP qt_qremoveWidgetFromStack(SEXP x, SEXP index);
    SEXP qt_qcurrentIndex(SEXP x);
    SEXP qt_qsetCurrentIndex(SEXP x, SEXP index);

    // basic.cpp

    SEXP qt_qwidget();
    SEXP qt_qpushButton(SEXP s);
    SEXP qt_qsetTextButton(SEXP x, SEXP s);
    SEXP qt_qtextButton(SEXP x);
    SEXP qt_qlabel(SEXP s);
    SEXP qt_qsetTextLabel(SEXP x, SEXP s);
    SEXP qt_qtextLabel(SEXP x);
    SEXP qt_qlineEdit(SEXP s);
    SEXP qt_qsetTextLineEdit(SEXP x, SEXP s);
    SEXP qt_qtextLineEdit(SEXP x);
    SEXP qt_qcheckBox(SEXP label);
    SEXP qt_qisCheckedButton(SEXP x);
    SEXP qt_qsetCheckedButton(SEXP x, SEXP status);


    // actions.cpp

    SEXP qt_qaction(SEXP desc, SEXP shortcut, SEXP parent);
    SEXP qt_qaddActionToQWidget(SEXP w, SEXP a);
    SEXP qt_qaddActionToQMenu(SEXP w, SEXP a);
    SEXP qt_qsetContextMenuPolicy(SEXP x, SEXP policy);


    // layout.cpp

    SEXP qt_qlayout(SEXP x);
    SEXP qt_qsetLayout(SEXP x, SEXP layout);
    SEXP qt_qsetContentsMarginsLayout(SEXP x, SEXP left, SEXP top, SEXP right, SEXP bottom);
    SEXP qt_qsetSpacing(SEXP x, SEXP spacing);
    SEXP qt_qsetVerticalSpacing(SEXP x, SEXP spacing);
    SEXP qt_qsetHorizontalSpacing(SEXP x, SEXP spacing);
    SEXP qt_qaddWidgetToLayout(SEXP x, SEXP widget, SEXP row, SEXP column, SEXP nrow, SEXP ncolumn);
    SEXP qt_qremoveWidgetFromLayout(SEXP x, SEXP widget);
    SEXP qt_qaddLayoutToLayout(SEXP x, SEXP layout, SEXP row, SEXP column, SEXP nrow, SEXP ncolumn);
    SEXP qt_qcolumnCount(SEXP x);
    SEXP qt_qrowCount(SEXP x);

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


    CALLDEF(qt_qshow, 1),
    CALLDEF(qt_qupdate, 1),
    CALLDEF(qt_qclose, 1),
    CALLDEF(qt_qraise, 1),
    CALLDEF(qt_qlower, 1),
    CALLDEF(qt_qparent, 1),
    CALLDEF(qt_qsetParent, 2),
    CALLDEF(qt_qresize, 3),
    CALLDEF(qt_qheight, 1),
    CALLDEF(qt_qwidth, 1),
    CALLDEF(qt_qsetMinimumSize, 3),
    CALLDEF(qt_setExpanding, 3),
    CALLDEF(qt_qsetContentsMargins, 5),

    CALLDEF(qt_qisEnabled, 1),
    CALLDEF(qt_qsetEnabled, 2),
    CALLDEF(qt_qwindowTitle, 1),
    CALLDEF(qt_qsetWindowTitle, 2),
    CALLDEF(qt_qrender, 2),
    CALLDEF(qt_qrenderGraphicsView, 1),
    CALLDEF(qt_qrenderToPixmap, 2),
    CALLDEF(qt_qrenderToSVG, 2),
    CALLDEF(qt_qsetStyleSheet, 2),
    CALLDEF(qt_qstyleSheet, 1),

//     CALLDEF(showWidget, 1),
//     CALLDEF(closeWidget, 1),
//     CALLDEF(unparentWidget, 1),
//     CALLDEF(setWidgetSize, 3),
//     CALLDEF(setWidgetMinimumSize, 3),
//     CALLDEF(renderWidget, 2),
//     CALLDEF(renderViewWidget, 1),
//     CALLDEF(renderWidgetToPixmap, 2),
//     CALLDEF(renderWidgetToSVG, 2),

    CALLDEF(addQtEventHandler, 0),
    CALLDEF(cleanupQtApp, 0),

    CALLDEF(newLabelWidget, 1),

    CALLDEF(qt_qtabWidget, 0),
    CALLDEF(qt_qaddTab, 4),
    CALLDEF(qt_qremoveTab, 2),
    CALLDEF(qt_qstackedWidget, 0),
    CALLDEF(qt_qaddWidgetToStack, 3), 
    CALLDEF(qt_qremoveWidgetFromStack, 2),
    CALLDEF(qt_qcurrentIndex, 1),
    CALLDEF(qt_qsetCurrentIndex, 2),

    CALLDEF(qt_qwidget, 0),
    CALLDEF(qt_qpushButton, 1),
    CALLDEF(qt_qsetTextButton, 2),
    CALLDEF(qt_qtextButton, 1),
    CALLDEF(qt_qlabel, 1),
    CALLDEF(qt_qsetTextLabel, 2),
    CALLDEF(qt_qtextLabel, 1),
    CALLDEF(qt_qlineEdit, 1),
    CALLDEF(qt_qsetTextLineEdit, 2),
    CALLDEF(qt_qtextLineEdit, 1),
    CALLDEF(qt_qcheckBox, 1),
    CALLDEF(qt_qisCheckedButton, 1),
    CALLDEF(qt_qsetCheckedButton, 2),

    CALLDEF(qt_qaction, 3),
    CALLDEF(qt_qaddActionToQWidget, 2),
    CALLDEF(qt_qaddActionToQMenu, 2),
    CALLDEF(qt_qsetContextMenuPolicy, 2),

    CALLDEF(qt_qlayout, 1),
    CALLDEF(qt_qsetLayout, 2),
    CALLDEF(qt_qsetContentsMarginsLayout, 5),
    CALLDEF(qt_qsetSpacing, 2),
    CALLDEF(qt_qsetVerticalSpacing, 2),
    CALLDEF(qt_qsetHorizontalSpacing, 2),
    CALLDEF(qt_qaddWidgetToLayout, 6),
    CALLDEF(qt_qremoveWidgetFromLayout, 2),
    CALLDEF(qt_qaddLayoutToLayout, 6),
    CALLDEF(qt_qcolumnCount, 1),
    CALLDEF(qt_qrowCount, 1),

    CALLDEF(qt_qsetDeleteOnClose, 2),

    {NULL, NULL, 0}
};



void R_init_qtbase(DllInfo *dll)
{
    init_utils(); // initializes some things unrelated to event handling

    // Register C routines
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);

    // Register for calling by other packages. FIXME: not sure how this works
    R_RegisterCCallable("qtbase", "wrapQWidget", (DL_FUNC) wrapQWidget);
    R_RegisterCCallable("qtbase", "wrapQObject", (DL_FUNC) wrapQObject);
    R_RegisterCCallable("qtbase", "unwrapQObjectReferee", (DL_FUNC) unwrapQObjectReferee);
}

    
SEXP 
addQtEventHandler()
{
    // at most one qApp can be running at a time
    if (!qApp) {
	R_Qt_init();

#ifndef WIN32

	R_PolledEvents = R_Qt_eventHandler;
	if (R_wait_usec > 10000 || R_wait_usec == 0)
	    R_wait_usec = 10000;

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








