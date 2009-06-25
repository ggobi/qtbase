#include <QMetaObject>
#include <QMetaMethod>

#include "RDynamicQObject.hpp"

extern "C" {
  SEXP qt_qconnect(SEXP x, SEXP which, SEXP handler, SEXP user_data, 
                   SEXP has_user_data)
  {
    RDynamicQObject *obj;
    const char *signal = CHAR(asChar(which));
    QObject *sender = unwrapQObject(x, QObject);
    const QMetaObject *meta = sender->metaObject();
    QMetaMethod method = meta->method(meta->indexOfSignal(signal));
    QList<QByteArray> paramTypes = method.parameterTypes();
    QByteArray returnType = QByteArray(method.typeName());
    if (!asLogical(has_user_data))
      user_data = NULL;
    obj = new RDynamicQObject(paramTypes, returnType, handler, user_data);
    obj->connectDynamicSlot(sender, signal, signal);
    return wrapQObject(obj);
  }
  
  SEXP qt_qdisconnect(SEXP x, SEXP receiver)
  {
    if (receiver == R_NilValue) {
      bool status = unwrapQObject(x, QObject)->disconnect();
      if (!status) warning("Disconnect unsuccessful");
    }
    else {
      QObject *obj = unwrapQObject(receiver, QObject);
      bool status = unwrapQObject(x, QObject)->disconnect(obj);
      if (!status) warning("Disconnect unsuccessful");
    }
    return R_NilValue;
  }
}
