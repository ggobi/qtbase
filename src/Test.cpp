
#include <QString>
#include <QLabel>

#include "wrappers.h"

static
QString sexp2qstring(SEXP s) {
    return QString::fromLocal8Bit(CHAR(asChar(s)));
}


extern "C" {
    SEXP newLabelWidget(SEXP label);
}


SEXP 
newLabelWidget(SEXP label) {
    QLabel *lab = new QLabel(sexp2qstring(label), 0);
    // lab->show();
    return wrapQWidget(lab);
}




