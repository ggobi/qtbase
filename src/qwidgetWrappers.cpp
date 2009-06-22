
#include <QApplication>
#include <QObject>
#include <QWidget>
#include <QPrinter>
#include <QPrintDialog>
#include <QPainter>
#include <QGraphicsView>
#include <QLabel>
#include <QPixmap>
#include <QSvgGenerator>

#include <QSize>
#include <QFont>
#include <QFontMetrics>

#include "wrappers.h"
#include "utils.hpp"

extern QApplication *app;

extern "C" {

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

}

SEXP
qt_qshow(SEXP x)
{
    unwrapQObject(x, QWidget)->show();
    return R_NilValue;
}

SEXP
qt_qupdate(SEXP x)
{
    // unwrapQObject(x, QWidget)->update();
    unwrapQObject(x, QWidget)->repaint();
    return R_NilValue;
}

SEXP
qt_qclose(SEXP x)
{
    unwrapQObject(x, QWidget)->close();
    return R_NilValue;
}

SEXP
qt_qraise(SEXP x)
{
    unwrapQObject(x, QWidget)->raise();
    return R_NilValue;
}

SEXP
qt_qlower(SEXP x)
{
    unwrapQObject(x, QWidget)->lower();
    return R_NilValue;
}

SEXP
qt_qparent(SEXP x)
{
    QWidget *p = unwrapQObject(x, QWidget)->parentWidget();
    if (p) 
	return wrapQWidget(p);
    else 
	return R_NilValue;
}

SEXP
qt_qsetParent(SEXP x, SEXP parent)
{
    if (parent == R_NilValue)
	unwrapQObject(x, QWidget)->setParent(0);
    else 
	unwrapQObject(x, QWidget)->setParent(unwrapQObject(parent, QWidget));
    return R_NilValue;
}

SEXP
qt_qresize(SEXP x, SEXP w, SEXP h)
{
    QWidget *e = unwrapQObject(x, QWidget);
    int ww, hh;
    if (w == R_NilValue) ww = e->width();
    else ww = asInteger(w);
    if (h == R_NilValue) hh = e->height();
    else hh = asInteger(h);
    e->resize(ww, hh);
    return R_NilValue;
}

SEXP
qt_qheight(SEXP x)
{
    return ScalarInteger(unwrapQObject(x, QWidget)->height());
}

SEXP
qt_qwidth(SEXP x)
{
    return ScalarInteger(unwrapQObject(x, QWidget)->width());
}

SEXP
qt_qsetMinimumSize(SEXP x, SEXP w, SEXP h)
{
    QWidget *e = unwrapQObject(x, QWidget);
    if (w != R_NilValue)
	e->setMinimumWidth(asInteger(w));
    if (h != R_NilValue) 
	e->setMinimumHeight(asInteger(h));
    return R_NilValue;
}

SEXP 
qt_setExpanding(SEXP x, SEXP vertical, SEXP horizontal)
{
    QWidget *w = unwrapQObject(x, QWidget);
    QSizePolicy policy = w->sizePolicy();
    if (vertical != R_NilValue) {
	if (asInteger(vertical))
	    policy.setVerticalPolicy(QSizePolicy::Expanding);
	else 
	    policy.setVerticalPolicy(QSizePolicy::Preferred);
    }
    if (horizontal != R_NilValue) {
	if (asInteger(horizontal))
	    policy.setHorizontalPolicy(QSizePolicy::Expanding);
	else 
	    policy.setHorizontalPolicy(QSizePolicy::Preferred);
    }
    w->setSizePolicy(policy);
    return x;
}


SEXP
qt_qsetContentsMargins(SEXP x, SEXP left, SEXP top, SEXP right, SEXP bottom)
{
    unwrapQObject(x, QWidget)->
	setContentsMargins(asInteger(left),
			   asInteger(top),
			   asInteger(right),
			   asInteger(bottom));
    return R_NilValue;
}


SEXP
qt_qisEnabled(SEXP x)
{
    if (unwrapQObject(x, QWidget)->isEnabled())
	return ScalarLogical(TRUE);
    else 
	return ScalarLogical(FALSE);
}

SEXP
qt_qsetEnabled(SEXP x, SEXP flag)
{
    if (asInteger(flag))
	unwrapQObject(x, QWidget)->setEnabled(true);
    else 
	unwrapQObject(x, QWidget)->setEnabled(false);
    return R_NilValue;
}

SEXP
qt_qwindowTitle(SEXP x)
{
    return qstring2sexp(unwrapQObject(x, QWidget)->windowTitle());
}

SEXP
qt_qsetWindowTitle(SEXP x, SEXP title)
{
    unwrapQObject(x, QWidget)->setWindowTitle(sexp2qstring(title));
    return R_NilValue;
}


// SEXP
// finalizeWidget(SEXP x)
// {
//     delete asQWidget(x);
//     R_ClearExternalPtr(x);
//     return R_NilValue;
// }


SEXP
qt_qrender(SEXP x, SEXP file)
{
    QWidget *w = unwrapQObject(x, QWidget);
    if (w) {
	QPrinter printer(QPrinter::ScreenResolution);
	if (file != R_NilValue) {
	    printer.setOutputFileName(sexp2qstring(file));
	    printer.setPaperSize(QSizeF::QSizeF(7.0, 7.0), QPrinter::Inch);
	}
	else {
	    QPrintDialog *printDialog = new QPrintDialog(&printer, 0);
	    if (printDialog->exec() != QDialog::Accepted) return R_NilValue;
	    printer.setPaperSize(QPrinter::A4);
	}
	QPainter painter(&printer);
	QRect rect = printer.pageRect();
	qreal pscale = 1.0 * rect.width() / w->width();
	if (pscale > 1.0 * rect.height() / w->height())
	    pscale = 1.0 * rect.height() / w->height();
	painter.scale(pscale, pscale);
	// painter.setRenderHint(QPainter::Antialiasing, false);
	w->render(&painter, QPoint(), QRegion(), QWidget::DrawChildren);
    }
    return R_NilValue;
}


SEXP
qt_qrenderGraphicsView(SEXP x)
{
    QGraphicsView *view = unwrapQObject(x, QGraphicsView);
    if (view) {
	QPrinter printer(QPrinter::ScreenResolution);
	QPrintDialog *printDialog = new QPrintDialog(&printer, 0);
	if (printDialog->exec() == QDialog::Accepted) {
	    printer.setPageSize(QPrinter::A4);
	    QPainter painter(&printer);
	    view->render(&painter);
	}
    }
    else { // FIXME: not needed any more since unwrapQObject will raise error
	warning("Cannot render as a view: invalid object");
    }
    return R_NilValue;
}


SEXP
qt_qrenderToPixmap(SEXP x, SEXP file)
{
    QWidget *w = unwrapQObject(x, QWidget);

//     QPixmap pixmap(w->size());
//     //w->render(&pixmap);
//     w->render(&pixmap, QPoint(), QRegion(), QWidget::DrawChildren);

    QPixmap pixmap = QPixmap::grabWidget(w);

    if (file == R_NilValue) {
	QLabel *label = new QLabel(0);
	label->setPixmap(pixmap);
	label->show();
    }
    else {
	if (!pixmap.save(sexp2qstring(file))) {
	    warning("Saving pixmap to file failed");
	};
    }
    return R_NilValue;
}


SEXP
qt_qrenderToSVG(SEXP x, SEXP file)
{
    if (file == R_NilValue) return R_NilValue;
    QWidget *w = unwrapQObject(x, QWidget);
    if (w) {
	QSvgGenerator svggen;
	svggen.setFileName(sexp2qstring(file));
	QPainter painter(&svggen);
	w->render(&painter, QPoint(), QRegion(), QWidget::DrawChildren);
    }
    return R_NilValue;
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


