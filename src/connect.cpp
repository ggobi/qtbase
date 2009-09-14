#include <smoke/qt_smoke.h>

#include "RDynamicQObject.hpp"
#include "SmokeObject.hpp"

#include <Rinternals.h>

extern "C" {
  SEXP qt_qconnect(SEXP x, SEXP which, SEXP handler, SEXP user_data, 
                   SEXP has_user_data)
  {
    RDynamicQObject *obj;
    const char *signal = CHAR(asChar(which));
    SmokeObject *so = SmokeObject::fromSexp(x);
    QObject *sender = reinterpret_cast<QObject *>(so->castPtr("QObject"));
    const QMetaObject *meta = sender->metaObject();
    MocMethod method(so->smoke(), meta, meta->indexOfSignal(signal));
    if (!asLogical(has_user_data))
      user_data = NULL;
    obj = new RDynamicQObject(method, handler, user_data, sender);
    obj->connectDynamicSlot(sender, signal, signal);
    // just return as an ordinary QObject, supporting disconnect()
    // the memory is owned by the QObject 'x'
    return SmokeObject::sexpFromPtr(obj, qt_Smoke, "QObject");
  }
}
