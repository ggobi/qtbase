#include <QObject>
#include <QMetaObject>
#include <QMetaMethod>

#include "wrap.hpp"

#include "SmokeObject.hpp"
#include <smoke/qt_smoke.h>

extern "C" {
  SEXP qt_qmocMethods(SEXP x) {
    QObject *self = unwrapSmoke(x, QObject);
    const QMetaObject *meta = self->metaObject();
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
      SET_STRING_ELT(ans_signature, i, mkChar(metaMethod.signature()));
      SET_STRING_ELT(ans_return, i, mkChar(metaMethod.typeName()));
      INTEGER(ans_nargs)[i] = metaMethod.parameterNames().size();
    }

    UNPROTECT(1);
    return ans;
  }
  
  SEXP qt_qnormalizedSignature(SEXP x) {
    QByteArray sig = QMetaObject::normalizedSignature(CHAR(asChar(x)));
    return sig.isEmpty() ? R_NilValue : mkString(sig.data());
  }

  SEXP qt_qproperties(SEXP x) {
    QObject *self = unwrapSmoke(x, QObject);
    const QMetaObject *meta = self->metaObject();
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
}



static QMetaObject* 
parent_meta_object(SEXP obj) 
{
  SmokeObject* o = SmokeObject::fromSexp(obj);
  Smoke *smoke = o->smoke();
  Smoke::ModuleIndex nameId = smoke->idMethodName("metaObject");
  Smoke::ModuleIndex classIdx(smoke, o->classId());
  Smoke::ModuleIndex meth = smoke->findMethod(classIdx, nameId);
  if (meth.index <= 0) {
    // Should never happen..
  }

  Smoke::Method &methodId =
    meth.smoke->methods[meth.smoke->methodMaps[meth.index].method];
  Smoke::ClassFn fn = smoke->classes[methodId.classId].classFn;
  Smoke::StackItem i[1];
  (*fn)(methodId.method, o->ptr(), i);
  return (QMetaObject*) i[0].s_voidp;
}

/* Presumably R will pass the metadata blob at class definition time,
   and store QMetaObject reference in the class environment. When the
   qt_metacall() virtual method is invoked, we retrieve the reference.
*/
SEXP
qt_qnewMetaObject(SEXP obj, SEXP parentMeta, SEXP rstringdata, SEXP rdata)
{
  QMetaObject* superdata = 0;

  if (parentMeta == R_NilValue) {
    // The parent class is a Smoke class, so call metaObject() on the
    // instance to get it via a smoke library call
    superdata = parent_meta_object(obj);
  } else {
    // The parent class is a custom R class whose metaObject
    // was constructed at runtime
    SmokeObject* p = SmokeObject::fromSexp(parentMeta);
    superdata = reinterpret_cast<QMetaObject *>(p->ptr());
  }

  char *stringdata = new char[length(rstringdata)];
  memcpy((void *) stringdata, RAW(rstringdata), length(rstringdata));
  
  int count = length(rdata);
  uint * data = new uint[count];
  for (long i = 0; i < count; i++) {
    data[i] = REAL(rdata)[i];
  }
  
  QMetaObject ob = { 
    { superdata, stringdata, data, 0 }
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
         "       %d,   %d, // enums/sets\n",
         data[0], data[1], data[2], data[3], 
         data[4], data[5], data[6], data[7], data[8], data[9]);

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
      printf("\n // signals: signature, parameters, type, tag, flags\n");
      signal_headings = false;
    } 

    if (slot_headings && (data[s + (j * 5) + 4] & 0x08) != 0) {
      printf("\n // slots: signature, parameters, type, tag, flags\n");
      slot_headings = false;
    }

    printf("      %d,   %d,   %d,   %d, 0x%2.2x\n", 
           data[s + (j * 5)], data[s + (j * 5) + 1], data[s + (j * 5) + 2], 
           data[s + (j * 5) + 3], data[s + (j * 5) + 4]);
  }

  s += (data[4] * 5);
  for (uint j = 0; j < data[6]; j++) {
    printf("\n // properties: name, type, flags\n");
    printf("      %d,   %d,   0x%8.8x\n", 
           data[s + (j * 3)], data[s + (j * 3) + 1], data[s + (j * 3) + 2]);
  }

  s += (data[6] * 3);
  for (int i = s; i < count; i++) {
    printf("\n       %d        // eod\n", data[i]);
  }

  printf("\nqt_meta_stringdata:\n    \"");

  int strlength = 0;
  for (int j = 0; j < RSTRING_LEN(stringdata_value); j++) {
    strlength++;
    if (meta->d.stringdata[j] == 0) {
      printf("\\0");
      if (strlength > 40) {
        printf("\"\n    \"");
        strlength = 0;
      }
    } else {
      printf("%c", meta->d.stringdata[j]);
    }
  }
  printf("\"\n\n");

#endif
  return SmokeObject::sexpFromPtr(meta, qt_Smoke, "QMetaObject", true);
}
