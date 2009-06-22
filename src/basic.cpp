
#include <QWidget>
#include <QAbstractButton>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>


#include "wrappers.h"
#include "utils.hpp"

extern "C" {

    SEXP qt_qwidget();
    SEXP qt_qpushButton(SEXP s);
    SEXP qt_qsetTextButton(SEXP x, SEXP s);
    SEXP qt_qtextButton(SEXP x);
    SEXP qt_qlabel(SEXP label);
    SEXP qt_qsetTextLabel(SEXP x, SEXP s);
    SEXP qt_qtextLabel(SEXP x);
    SEXP qt_qlineEdit(SEXP s);
    SEXP qt_qsetTextLineEdit(SEXP x, SEXP s);
    SEXP qt_qtextLineEdit(SEXP x);
    SEXP qt_qcheckBox(SEXP label);
    SEXP qt_qisCheckedButton(SEXP x);
    SEXP qt_qsetCheckedButton(SEXP x, SEXP status);

}

// Next in line to implement:

// QComboBox	Combined button and popup list




// QCheckBox	Checkbox with a text label
// QComboBox	Combined button and popup list
// QDial	Rounded range control (like a speedometer or potentiometer)
// QDoubleSpinBox	Spin box widget that takes doubles
// QFontComboBox	Combobox that lets the user select a font family
// QLCDNumber	Displays a number with LCD-like digits
// QLabel	Text or image display
// QLineEdit	One-line text editor
// QMenu	Menu widget for use in menu bars, context menus, and other popup menus
// QProgressBar	Horizontal or vertical progress bar
// QPushButton	Command button
// QRadioButton	Radio button with a text label
// QScrollArea	Scrolling view onto another widget
// QScrollBar	Vertical or horizontal scroll bar
// QSlider	Vertical or horizontal slider
// QSpinBox	Spin box widget
// QTabBar	Tab bar, e.g. for use in tabbed dialogs
// QTimeEdit	Widget for editing times based on the QDateTimeEdit widget
// QToolBox	Column of tabbed widget items
// QToolButton	Quick-access button to commands or options, usually used inside a QToolBar

SEXP 
qt_qwidget() {
    return wrapQWidget(new QWidget());
}


SEXP 
qt_qpushButton(SEXP s) {
    return wrapQWidget(new QPushButton(sexp2qstring(s)));
}

SEXP 
qt_qsetTextButton(SEXP x, SEXP s) {
    unwrapQObject(x, QAbstractButton)->setText(sexp2qstring(s));
    return R_NilValue;
}

SEXP 
qt_qtextButton(SEXP x) {
    return qstring2sexp(unwrapQObject(x, QAbstractButton)->text());
}

SEXP 
qt_qlabel(SEXP label) {
    QLabel *x = new QLabel(sexp2qstring(label), 0);
    return wrapQWidget(x);
}

SEXP 
qt_qsetTextLabel(SEXP x, SEXP s) {
    unwrapQObject(x, QLabel)->setText(sexp2qstring(s));
    return R_NilValue;
}

SEXP 
qt_qtextLabel(SEXP x) {
    return qstring2sexp(unwrapQObject(x, QLabel)->text());
}


SEXP 
qt_qlineEdit(SEXP s) {
    QLineEdit *x = new QLineEdit();
    if (s != R_NilValue) x->setText(sexp2qstring(s));
    return wrapQWidget(x);
}

SEXP 
qt_qsetTextLineEdit(SEXP x, SEXP s) {
    unwrapQObject(x, QLineEdit)->setText(sexp2qstring(s));
    return R_NilValue;
}

SEXP 
qt_qtextLineEdit(SEXP x) {
    return qstring2sexp(unwrapQObject(x, QLineEdit)->text());
}

SEXP 
qt_qcheckBox(SEXP label) {
    if (label == R_NilValue) return wrapQWidget(new QCheckBox(0));
    else return wrapQWidget(new QCheckBox(sexp2qstring(label), 0));
}

SEXP 
qt_qisCheckedButton(SEXP x) {
    if (unwrapQObject(x, QAbstractButton)->isChecked())
	return ScalarLogical(TRUE);
    else 
	return ScalarLogical(FALSE);
}

SEXP 
qt_qsetCheckedButton(SEXP x, SEXP status) {
    if (asInteger(status))
	unwrapQObject(x, QAbstractButton)->setChecked(true);
    else 
	unwrapQObject(x, QAbstractButton)->setChecked(false);
    return x;
}




