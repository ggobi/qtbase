
#include <QWidget>
#include <QTabWidget>
#include <QStackedWidget>

#include "wrappers.h"

static
QString sexp2qstring(SEXP s) {
    return QString::fromLocal8Bit(CHAR(asChar(s)));
}

extern "C" {
    SEXP qt_qtabWidget();
    SEXP qt_qaddTab(SEXP x, SEXP tab, SEXP label, SEXP index);
    SEXP qt_qremoveTab(SEXP x, SEXP index);
    SEXP qt_qstackedWidget();
    SEXP qt_qaddWidgetToStack(SEXP x, SEXP w, SEXP index);
    SEXP qt_qremoveWidgetFromStack(SEXP x, SEXP index);
    SEXP qt_qcurrentIndex(SEXP x);
    SEXP qt_qsetCurrentIndex(SEXP x, SEXP index);
}


// QTabWidget

SEXP 
qt_qtabWidget() {
    QTabWidget *tabw = new QTabWidget();
    return wrapQWidget(tabw);
}

SEXP 
qt_qaddTab(SEXP x, SEXP tab, SEXP label, SEXP index) {
    int i = asInteger(index);
    if (i <= 0)
	unwrapQObject(x, QTabWidget)->addTab(unwrapQObject(tab, QWidget), sexp2qstring(label));
    else 
	unwrapQObject(x, QTabWidget)->insertTab(i - 1, unwrapQObject(tab, QWidget), sexp2qstring(label));
    return R_NilValue;
}

SEXP 
qt_qremoveTab(SEXP x, SEXP index) {
    int i = asInteger(index);
    if (i > 0) unwrapQObject(x, QTabWidget)->removeTab(i - 1);
    return R_NilValue;
}

// QStackedWidget

SEXP 
qt_qstackedWidget() {
    QStackedWidget *s = new QStackedWidget();
    return wrapQWidget(s);
}

SEXP 
qt_qaddWidgetToStack(SEXP x, SEXP w, SEXP index) {
    int i = asInteger(index);
    if (i <= 0)
	unwrapQObject(x, QStackedWidget)->addWidget(unwrapQObject(w, QWidget));
    else 
	unwrapQObject(x, QStackedWidget)->insertWidget(i - 1, unwrapQObject(w, QWidget));
    return R_NilValue;
}

SEXP 
qt_qremoveWidgetFromStack(SEXP x, SEXP index) {
    int i = asInteger(index);
    QStackedWidget *s = unwrapQObject(x, QStackedWidget);
    QWidget *w;
    if (i > 0 && i <= s->count()) {
	w = s->widget(i - 1);
	s->removeWidget(w);
	return wrapQWidget(w);
    }    
    return R_NilValue;
}

// common

SEXP 
qt_qcurrentIndex(SEXP x) {
    QWidget *w = unwrapQObject(x, QWidget);
    if (qobject_cast<QTabWidget *>(w))
	return ScalarInteger(1 + unwrapQObject(x, QTabWidget)->currentIndex());
    else if (qobject_cast<QStackedWidget *>(w))
	return ScalarInteger(1 + unwrapQObject(x, QStackedWidget)->currentIndex());
    else 
	return ScalarInteger(0);
}

SEXP 
qt_qsetCurrentIndex(SEXP x, SEXP index) {
    SEXP ans = PROTECT(qt_qcurrentIndex(x));
    int i = asInteger(index);
    QWidget *w = unwrapQObject(x, QWidget);
    if (qobject_cast<QTabWidget *>(w))
	unwrapQObject(x, QTabWidget)->setCurrentIndex(i - 1);
    else if (qobject_cast<QStackedWidget *>(w))
	unwrapQObject(x, QStackedWidget)->setCurrentIndex(i - 1);
    UNPROTECT(1);
    return ans;
}

