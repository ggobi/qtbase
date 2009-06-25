#include <QObject>
#include <QMetaObject>
#include <QMetaMethod>

#include "wrappers.h"

extern "C" {
  SEXP qt_qmethods(SEXP rself) {
    QObject *self = unwrapQObject(rself, QObject);
    const QMetaObject *meta = self->metaObject();
    int n = meta->methodCount();
    
    SEXP ans, ans_type, ans_signature, ans_return;
    PROTECT(ans = allocVector(VECSXP, 3));
    ans_type = allocVector(INTSXP, n);
    SET_VECTOR_ELT(ans, 0, ans_type);
    ans_signature = allocVector(STRSXP, n);
    SET_VECTOR_ELT(ans, 1, ans_signature);
    ans_return = allocVector(STRSXP, n);
    SET_VECTOR_ELT(ans, 2, ans_return);
    
    for (int i = 0; i < n; i++) {
      QMetaMethod metaMethod = meta->method(i);
      INTEGER(ans_type)[i] = metaMethod.methodType();
      SET_STRING_ELT(ans_signature, i, mkChar(metaMethod.signature()));
      SET_STRING_ELT(ans_return, i, mkChar(metaMethod.typeName()));
    }

    UNPROTECT(1);
    return ans;
  }
  
  SEXP qt_qnormalizedSignature(SEXP x) {
    QByteArray sig = QMetaObject::normalizedSignature(CHAR(asChar(x)));
    return sig.isEmpty() ? R_NilValue : mkString(sig.data());
  }
}
