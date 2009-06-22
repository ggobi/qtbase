
#include <QObject>
#include <QWidget>
#include <QLayout>
#include <QGridLayout>

#include "wrappers.h"
#include "utils.hpp"

extern "C" {
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
}

SEXP
qt_qlayout(SEXP x)
{
    if (x == R_NilValue)
	return wrapQObject(new QGridLayout());
    else {
	QLayout *l = unwrapQObject(x, QWidget)->layout();
	if (l) return wrapQObject(l);
	else return R_NilValue;
    }
}

SEXP
qt_qsetLayout(SEXP x, SEXP layout)
{
    unwrapQObject(x, QWidget)->setLayout(unwrapQObject(layout, QLayout));
    return R_NilValue;
}


SEXP
qt_qsetContentsMarginsLayout(SEXP x, SEXP left, SEXP top, SEXP right, SEXP bottom)
{
    unwrapQObject(x, QLayout)->
	setContentsMargins(asInteger(left),
			   asInteger(top),
			   asInteger(right),
			   asInteger(bottom));
    return R_NilValue;
}


SEXP
qt_qsetSpacing(SEXP x, SEXP spacing)
{
    unwrapQObject(x, QGridLayout)->setSpacing(asInteger(spacing));
    return R_NilValue;
}

SEXP
qt_qsetVerticalSpacing(SEXP x, SEXP spacing)
{
    unwrapQObject(x, QGridLayout)->setVerticalSpacing(asInteger(spacing));
    return R_NilValue;
}

SEXP
qt_qsetHorizontalSpacing(SEXP x, SEXP spacing)
{
    unwrapQObject(x, QGridLayout)->setHorizontalSpacing(asInteger(spacing));
    return R_NilValue;
}

SEXP
qt_qaddWidgetToLayout(SEXP x, SEXP widget, 
		      SEXP row, SEXP column, 
		      SEXP nrow, SEXP ncolumn)
{
    unwrapQObject(x, QGridLayout)->
	addWidget(unwrapQObject(widget, QWidget),
		  asInteger(row)-1, asInteger(column)-1,
		  asInteger(nrow), asInteger(ncolumn));
    return R_NilValue;
}

SEXP
qt_qremoveWidgetFromLayout(SEXP x, SEXP widget)
{
    unwrapQObject(x, QLayout)->removeWidget(unwrapQObject(widget, QWidget));
    return R_NilValue;
}


SEXP
qt_qaddLayoutToLayout(SEXP x, SEXP layout, 
		      SEXP row, SEXP column, 
		      SEXP nrow, SEXP ncolumn)
{
    unwrapQObject(x, QGridLayout)->
	addLayout(unwrapQObject(layout, QGridLayout),
		  asInteger(row)-1, asInteger(column)-1,
		  asInteger(nrow), asInteger(ncolumn));
    return R_NilValue;
}

SEXP
qt_qcolumnCount(SEXP x)
{
    return ScalarInteger(unwrapQObject(x, QGridLayout)->columnCount());
}

SEXP
qt_qrowCount(SEXP x)
{
    return ScalarInteger(unwrapQObject(x, QGridLayout)->rowCount());
}

// void QGridLayout::setOriginCorner ( Qt::Corner corner )

