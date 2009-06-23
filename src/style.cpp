
#include <QApplication>
#include <QString>

#include "wrappers.h"
#include "utils.hpp"

extern QApplication *app;

extern "C" {
    SEXP qt_qsetStyleSheet(SEXP x, SEXP s);
    SEXP qt_qstyleSheet(SEXP x);
}


SEXP 
qt_qsetStyleSheet(SEXP x, SEXP s)
{
    const QString style = sexp2qstring(s);
    if (x == R_NilValue)
	app->setStyleSheet(style);
    else 
	unwrapQObject(x, QWidget)->setStyleSheet(style);
    return R_NilValue;
}

SEXP 
qt_qstyleSheet(SEXP x)
{
    if (x == R_NilValue)
	return qstring2sexp(app->styleSheet());
    else 
	return qstring2sexp(unwrapQObject(x, QWidget)->styleSheet());
}

