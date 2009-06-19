
#include <QWidget>

#include "wrappers.h"

extern "C" {
    SEXP qt_qsetDeleteOnClose(SEXP x, SEXP status);
}


SEXP qt_qsetDeleteOnClose(SEXP x, SEXP status) 
{
    unwrapQObject(x, QWidget)->setAttribute(Qt::WA_DeleteOnClose, asInteger(status));
    return R_NilValue;
}


// A more general interface?

// static
// QString sexp2qstring(SEXP s) {
//     return QString::fromLocal8Bit(CHAR(asChar(s)));
// }

// extern "C" {

//     SEXP qt_qsetAttribute(SEXP x, SEXP attr, SEXP status);

// }

// SEXP
// qt_qsetAttribute(SEXP x, SEXP attr, SEXP status)
// {
//     Qt::WidgetAttribute wa;
//     QString a = sexp2qstring(attr);
//     if (a == "deleteonclose") 
// 	wa = Qt::WA_DeleteOnClose;
//     else if (a == "")
// 	wa = Qt::WA_AcceptDrops;
//     else if (a == "")
// 	wa = Qt::WA_Disabled;
//     unwrapQObject(x, QWidget)->setAttribute(wa, asInteger(status));
//     return R_NilValue;
// }



