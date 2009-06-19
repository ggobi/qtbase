
#include <QWidget>
#include <QMenu>
#include <QAction>

#include "wrappers.h"

static
QString sexp2qstring(SEXP s) {
    return QString::fromLocal8Bit(CHAR(asChar(s)));
}



extern "C" {

    SEXP qt_qaction(SEXP desc, SEXP shortcut, SEXP parent);
    SEXP qt_qaddActionToQWidget(SEXP w, SEXP a);
    SEXP qt_qaddActionToQMenu(SEXP w, SEXP a);
    SEXP qt_qsetContextMenuPolicy(SEXP x, SEXP policy);
}


SEXP
qt_qaction(SEXP desc, SEXP shortcut, SEXP parent)
{
    QAction *a;
    if (parent == R_NilValue)
	a = new QAction(sexp2qstring(desc), 0);
    else 
	a = new QAction(sexp2qstring(desc), unwrapQObject(parent, QWidget));
    if (shortcut != R_NilValue)
	a->setShortcut(sexp2qstring(shortcut));
    return wrapQObject(a);
}

SEXP
qt_qaddActionToQWidget(SEXP x, SEXP a)
{
    unwrapQObject(x, QWidget)->addAction(unwrapQObject(a, QAction));
    return R_NilValue;
}

SEXP
qt_qaddActionToQMenu(SEXP x, SEXP a)
{
    unwrapQObject(x, QMenu)->addAction(unwrapQObject(a, QAction));
    return R_NilValue;
}


//     connect(printAct, SIGNAL(triggered()), 
// 	    w, SLOT(print()));
//     w->addAction(printAct);
//     setContextMenuPolicy(Qt::ActionsContextMenu);


SEXP
qt_qsetContextMenuPolicy(SEXP x, SEXP policy)
{
    QString p = sexp2qstring(policy);
    QWidget *w = unwrapQObject(x, QWidget);
    if (p == "none") w->setContextMenuPolicy(Qt::NoContextMenu);
    else if (p == "prevent") w->setContextMenuPolicy(Qt::PreventContextMenu);
    else if (p ==  "default") w->setContextMenuPolicy(Qt::DefaultContextMenu);
    else if (p == "actions") w->setContextMenuPolicy(Qt::ActionsContextMenu);
    else if (p ==  "custom") w->setContextMenuPolicy(Qt::CustomContextMenu);
    return R_NilValue;
}

