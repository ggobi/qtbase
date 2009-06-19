#ifndef RQTCONVERT_CPP
#define RQTCONVERT_CPP

#include <R.h>
#include <Rinternals.h>
#include <QString>

QString sexp2qstring(SEXP s) {
    return QString::fromLocal8Bit(CHAR(asChar(s)));
}


extern "C" {

SEXP qstring2sexp(QString s) {
    return ScalarString(mkChar(s.toLocal8Bit().data()));
}

}


#endif
