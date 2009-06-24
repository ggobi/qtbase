
#include <QWidget>
#include <QMenu>
#include <QAction>

#include "wrappers.h"
#include "utils.hpp"

extern "C" {

    SEXP qt_qaction(SEXP desc, SEXP shortcut, SEXP parent, SEXP tooltip,
                    SEXP checkable);
    SEXP qt_qaddActionToQWidget(SEXP w, SEXP a);
    SEXP qt_qaddActionToQMenu(SEXP w, SEXP a);
    SEXP qt_qaddAction_QGraphicsWidget(SEXP w, SEXP a);
    SEXP qt_qsetContextMenuPolicy(SEXP x, SEXP policy);
    SEXP qt_qsetEnabled_QAction(SEXP raction, SEXP enabled);
    SEXP qt_qchecked_QAction(SEXP raction);
}


SEXP
qt_qaction(SEXP desc, SEXP shortcut, SEXP parent, SEXP tooltip,
           SEXP checkable)
{
    QAction *a;
    if (parent == R_NilValue)
	a = new QAction(sexp2qstring(desc), 0);
    else 
	a = new QAction(sexp2qstring(desc), unwrapQObject(parent, QWidget));
    if (shortcut != R_NilValue)
	a->setShortcut(sexp2qstring(shortcut));
    a->setCheckable(asLogical(checkable));
    a->setToolTip(sexp2qstring(tooltip));
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

SEXP qt_qsetEnabled_QAction(SEXP raction, SEXP enabled) {
  QAction *action = unwrapQObject(raction, QAction);
  action->setEnabled(asLogical(enabled));
  return R_NilValue;
}
SEXP qt_qchecked_QAction(SEXP raction) {
  QAction *action = unwrapQObject(raction, QAction);
  return ScalarLogical(action->isChecked());
}

SEXP qt_qaddAction_QGraphicsWidget(SEXP rself, SEXP raction) {
  QGraphicsWidget *item = unwrapQObject(rself, QGraphicsWidget);
  item->addAction(unwrapQObject(raction, QAction));
  return rself;
}
