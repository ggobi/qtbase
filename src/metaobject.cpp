#include <QObject>
#include <QMetaObject>
#include <QMetaMethod>

#include "MocMethod.hpp"
#include "MocClass.hpp"
#include "MocDynamicBinding.hpp"
#include "MocStack.hpp"
#include "SmokeStack.hpp"
#include "SmokeType.hpp"
#include "SmokeObject.hpp"
#include "Property.hpp"
#include "convert.hpp"

#include <smoke/qt_smoke.h>

extern "C" SEXP qt_qmocMethods(SEXP x) {
  const QMetaObject *meta = unwrapSmoke(x, QMetaObject);
  int n = meta->methodCount();
  
  SEXP ans, ans_type, ans_signature, ans_return, ans_nargs;
  PROTECT(ans = allocVector(VECSXP, 4));
  ans_type = allocVector(INTSXP, n);
  SET_VECTOR_ELT(ans, 0, ans_type);
  ans_signature = allocVector(STRSXP, n);
  SET_VECTOR_ELT(ans, 1, ans_signature);
  ans_return = allocVector(STRSXP, n);
  SET_VECTOR_ELT(ans, 2, ans_return);
  ans_nargs = allocVector(INTSXP, n);
  SET_VECTOR_ELT(ans, 3, ans_nargs);    
  
  for (int i = 0; i < n; i++) {
    QMetaMethod metaMethod = meta->method(i);
    INTEGER(ans_type)[i] = metaMethod.methodType();
    SET_STRING_ELT(ans_signature, i,
                   mkChar(metaMethod.methodSignature().constData()));
    SET_STRING_ELT(ans_return, i, mkChar(metaMethod.typeName()));
    INTEGER(ans_nargs)[i] = metaMethod.parameterNames().size();
  }
  
  UNPROTECT(1);
  return ans;
}
  
extern "C" SEXP qt_qnormalizedSignature(SEXP x) {
  QByteArray sig = QMetaObject::normalizedSignature(CHAR(asChar(x)));
  return sig.isEmpty() ? R_NilValue : mkString(sig.data());
}

extern "C" SEXP qt_qproperties(SEXP x) {
  const QMetaObject *meta = unwrapSmoke(x, QMetaObject);
  int n = meta->propertyCount();
  
  SEXP ans, ans_type, ans_name, ans_readable, ans_writable;
  PROTECT(ans = allocVector(VECSXP, 4));
  ans_name = allocVector(STRSXP, n);
  SET_VECTOR_ELT(ans, 0, ans_name);
  ans_type = allocVector(STRSXP, n);
  SET_VECTOR_ELT(ans, 1, ans_type);
  ans_readable = allocVector(LGLSXP, n);
  SET_VECTOR_ELT(ans, 2, ans_readable);
  ans_writable = allocVector(LGLSXP, n);
  SET_VECTOR_ELT(ans, 3, ans_writable);
  
  for (int i = 0; i < n; i++) {
    QMetaProperty metaProperty = meta->property(i);
    SET_STRING_ELT(ans_type, i, mkChar(metaProperty.typeName()));
    SET_STRING_ELT(ans_name, i, mkChar(metaProperty.name()));
    LOGICAL(ans_readable)[i] = metaProperty.isReadable();
    LOGICAL(ans_writable)[i] = metaProperty.isWritable();
  }
  
  UNPROTECT(1);
  return ans;
}

extern "C" SEXP qt_qmetaInvoke(SEXP x, SEXP s_id, SEXP s_args) {
  SmokeObject *so = SmokeObject::fromSexp(x);
  int id = from_sexp<int>(s_id);
  QObject * qobj = reinterpret_cast<QObject *>(so->castPtr("QObject"));
  MocMethod method(so->smoke(), qobj->metaObject(), id);
  SEXP ret = method.invoke(x, s_args);
  if (method.lastError() > Method::NoError)
    error("Meta method invocation failed for: '%s::%s'", so->klass()->name(),
          method.name());
  return ret;
}

/* We catch all qt_metacall invocations */
extern "C" SEXP qt_qmetacall(SEXP x, SEXP s_call, SEXP s_id, SEXP s_args)
{
  SmokeObject *so = SmokeObject::fromSexp(x);
  QMetaObject::Call call =
    enum_from_sexp<QMetaObject::Call>(s_call, SmokeType());
  int id = from_sexp<int>(s_id);
  void **args = reinterpret_cast<void **>(from_sexp<void *>(s_args));
  
  // Assume the target slot is a C++ one
  Smoke::StackItem i[4];
  i[1].s_enum = call;
  i[2].s_int = id;
  i[3].s_voidp = args;
  so->invokeMethod("qt_metacall$$?", i);
  int ret = i[0].s_int;
  if (ret < 0) {
    return ScalarInteger(ret);
  }

  QObject * qobj = reinterpret_cast<QObject *>(so->castPtr("QObject"));
  // get obj metaobject with a virtual call
  const QMetaObject *metaobject = qobj->metaObject();
  
  if (call == QMetaObject::ReadProperty || call == QMetaObject::WriteProperty) {
    MocStack mocStack = MocStack(args, 1);
    QMetaProperty metaProp = metaobject->property(id);
    Property *prop = so->klass()->property(metaProp.name());
    SmokeType type(so->smoke(), metaProp.typeName(), metaobject->className());
    if (call == QMetaObject::ReadProperty) {
      Smoke::StackItem item = prop->read(so);
      mocStack.returnFromSmoke(SmokeStack(&item, 1), type);
    } else if (call == QMetaObject::WriteProperty) {
      QVector<SmokeType> types;
      types += type;
      prop->write(so, mocStack.toSmoke(types).ret());
    }
  }

  if (call != QMetaObject::InvokeMetaMethod)
    return ScalarInteger(id);
  
  // get method count
  int count = metaobject->methodCount();
  
  QMetaMethod method = metaobject->method(id);
  if (method.methodType() == QMetaMethod::Signal) {
    // FIXME: this overload of 'activate' is obsolete
    metaobject->activate(qobj, id, (void**) args);
    return ScalarInteger(id - count);
  }
  
  MocDynamicBinding binding(MocMethod(so->smoke(), metaobject, id));
  binding.invoke(qobj, args);
  /*
    DynamicBinding binding(MocMethod(so->smoke(), metaobject, id));
    QVector<SmokeType> stackTypes = binding.types();
    MocStack mocStack = MocStack(args, stackTypes.size());
    SmokeStack smokeStack = mocStack.toSmoke(stackTypes);
    binding.invoke(so, smokeStack.items());
    mocStack.returnFromSmoke(smokeStack, stackTypes[0]);
  */
  if (binding.lastError() == Method::NoError)
    warning("Slot invocation failed for %s::%s", so->klass()->name(),
            binding.name());
  
  return ScalarInteger(id - count);
}

#undef DEBUG

/* Presumably R will pass the metadata blob at class definition time,
   and store QMetaObject reference. When the
   metaObject() virtual method is invoked, we retrieve the reference.
*/
extern "C" SEXP
qt_qnewMetaObject(SEXP x, SEXP rstringdata, SEXP rdata)
{
  const Class *cl = Class::fromSexp(x);
  const QMetaObject *superdata = MocClass(cl->parents()[0]).metaObject();

  QByteArrayData *bytearraydata = new QByteArrayData[length(rstringdata)];
  for (long i = 0; i < length(rstringdata); i++) {
    const char *str = qstrdup(CHAR(STRING_ELT(rstringdata, i)));
    int len = qstrlen(str);
    qptrdiff offset = (qptrdiff)str - (qptrdiff)(bytearraydata + i);
    const QByteArrayData &bd =
      Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, offset);
    memcpy(bytearraydata + i, &bd, sizeof(QByteArrayData));
  }
  
  int count = length(rdata);
  uint * data = new uint[count];
  for (long i = 0; i < count; i++) {
    data[i] = REAL(rdata)[i];
  }
  
  QMetaObject ob = { 
    { superdata, bytearraydata, data, 0 }
  } ;

  QMetaObject * meta = new QMetaObject;
  *meta = ob;

#ifdef DEBUG
  printf("qt_qnewMetaObject() superdata: %p %s\n", meta->d.superdata, superdata->className());
	
  printf(
         " // content:\n"
         "       %d,       // revision\n"
         "       %d,       // classname\n"
         "       %d,   %d, // classinfo\n"
         "       %d,   %d, // methods\n"
         "       %d,   %d, // properties\n"
         "       %d,   %d, // enums/sets\n"
         "       %d,   %d, // constructors\n"
         "       %d,       // flags\n"
         "       %d,       // signal count\n",
         data[0], data[1], data[2], data[3], 
         data[4], data[5], data[6], data[7], data[8], data[9],
         data[10], data[11], data[12], data[13]);

  int s = data[3];

  if (data[2] > 0) {
    printf("\n // classinfo: key, value\n");
    for (uint j = 0; j < data[2]; j++) {
      printf("      %d,    %d\n", data[s + (j * 2)], data[s + (j * 2) + 1]);
    }
  }

  s = data[5];
  bool signal_headings = true;
  bool slot_headings = true;

  for (uint j = 0; j < data[4]; j++) {
    if (signal_headings && (data[s + (j * 5) + 4] & 0x04) != 0) {
      printf("\n // signals: name, param-count, typeinfo-offset, tag, flags\n");
      signal_headings = false;
    } 

    if (slot_headings && (data[s + (j * 5) + 4] & 0x08) != 0) {
      printf("\n // slots: : name, param-count, typeinfo-offset, tag, flags\n");
      slot_headings = false;
    }

    printf("      %d,   %d,   %d,   %d, 0x%2.2x\n", 
           data[s + (j * 5)], data[s + (j * 5) + 1], data[s + (j * 5) + 2], 
           data[s + (j * 5) + 3], data[s + (j * 5) + 4]);
  }

  s += (data[4] * 5);
  printf("\n // properties: name, type, flags\n");
  for (uint j = 0; j < data[6]; j++) {
    printf("      %d,   %d,   0x%8.8x\n", 
           data[s + (j * 3)], data[s + (j * 3) + 1], data[s + (j * 3) + 2]);
  }

  s += (data[6] * 3);
  printf("\n // notifies: signal\n");
  for (uint i = 0; i < data[6]; i++) {
    printf("\n    %d\n", data[s + i]);
  }

  s += data[6];
  printf("\n // typeinfo: name, [param_type]*, [param_name]* \n    ");
  for (uint i = s; i < count; i++) {
    printf("%d, ", data[i]);
  }
  
  printf("\n\nqt_meta_stringdata:\n");

  for (int i = 0; i < length(rstringdata); i++) {
    printf("     \"%s\"\n", (const char *) meta->d.stringdata[i].data());
  }
  printf("\n");

#endif
  return SmokeObject::sexpFromPtr(meta, qt_Smoke, "QMetaObject", true);
}
