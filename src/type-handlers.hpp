#ifndef TYPE_HANDLERS_H
#define TYPE_HANDLERS_H

#include "MethodCall.hpp"
#include "SmokeObject.hpp"

#include "convert.hpp"

#undef isPrimitive

/*
  We need to handle four basic types:
  * Scalars (primitives, objects)
  * Collections (QList, QVector, QSet, ...)
  * Maps (QMap, QHash)
  * Pairs (QPair)

  Except for primitives, Smoke stores everything in a void
  pointer. For simple classes, we wrap the pointer with the class
  ID. However, for template types like the non-scalars and certain
  classes that require special conversion (e.g. QString), we want to
  convert based on type name. The QHash below serves this purpose.
*/

extern QHash<QByteArray, TypeHandler*> type_handlers;

/**************************** MARSHALLING  **************************/

/*
  When marshalling values, there are three cases:
  1) primitives on the stack (trivial)
  1) pointers to primitives
  2) values that need to become pointers to fit on the Smoke stack

  What are pointers to primitives: arrays or return by reference? This
  is very difficult to determine (RGtk2 required human
  annotation). Arrays are especially challenging, because their length
  is not easily learned (RGtk2 had heuristics, but manual intervention
  was often required). One might expect Qt to always use QVector/QLists
  instead of arrays, but the world is not that kind. Anyway, we will
  assume that primitive pointers are return-by-ref parameters. This
  reduces them to the second case, values that Smoke treats as pointers.

  Note that this means primitive strings, 'char*', will NOT be
  supported. Cursory analysis of the Qt API indicates that 'char*' is
  often used in the same way (e.g. via overloading) as
  QByteArray. This is probably only for C++ convenience (and for
  getting strings into QString and QByteArray in the first
  place). Thus, we will not worry about it.
  
  Values becoming pointers requires special logic. Many types fall
  into this category, including "long long", all value classes like
  QString that are specially handled, and primitive
  pointers/references.
  
  General pattern for conversion:
  R to Smoke:
  1) Convert the SEXP to C++, allocating if virtual return
  2) marshal()
  3) Possibly return pass-by-ref params to R
     
  Smoke To R:
  1) Convert C++ to SEXP
  2) marshal()
  3) Possibly return pass-by-ref params to C++ (virtual callbacks)
  4) Possibly free memory allocated by Smoke
*/

/* First a bunch of function templates for accessing the MethodCall */

template <typename T> T* itemPtr(MethodCall *m) {
  return (T*) &m->item().s_voidp;
}

/* primitive specializations */
template<> bool* itemPtr<bool>(MethodCall *m) { return &m->item().s_bool; }
template<> signed char* itemPtr<signed char>(MethodCall *m) {
  return &m->item().s_char;
}
template<> unsigned char* itemPtr<unsigned char>(MethodCall *m) {
  return &m->item().s_uchar;
}
template<> short* itemPtr<short>(MethodCall *m) { return &m->item().s_short; }
template<> unsigned short* itemPtr<unsigned short>(MethodCall *m) {
  return &m->item().s_ushort;
}
template<> int* itemPtr<int>(MethodCall *m) { return &m->item().s_int; }
template<> unsigned int* itemPtr<unsigned int>(MethodCall *m) {
  return &m->item().s_uint;
}
template<> long* itemPtr<long>(MethodCall *m) {
  return &m->item().s_long;
}
template<> unsigned long* itemPtr<unsigned long>(MethodCall *m) {
  return &m->item().s_ulong;
}
template<> float* itemPtr<float>(MethodCall *m) { return &m->item().s_float; }
template<> double* itemPtr<double>(MethodCall *m) {
  return &m->item().s_double;
}
template<> void* itemPtr<void>(MethodCall *m) { return m->item().s_voidp; }

/* high-level accessors */
template <typename T> T itemValue(MethodCall *m) {
  return *itemPtr<T>(m);
}
template <typename T> void setItemValue(MethodCall *m, T value) {
  *(itemPtr<T>(m)) = value;
}


template <typename T>
void marshal_from_sexp(MethodCall *m) 
{
  SEXP sexp = m->sexp();
  SmokeType t = m->type();
  T *qp = NULL, qv;

  if (t.isPrimitive()) { // catch primitives here
    setItemValue(m, from_sexp<T>(sexp, t));
    return;
  }
  
  if (!(t.isPtr() && sexp == R_NilValue)) {
    qv = from_sexp<T>(sexp, t);
    if (m->returning() && !t.fitsStack())
      qp = new T(qv); // when returning from virtual, smoke frees this
    else qp = &qv; // avoid need to delete by using stack
  }
  
  setItemValue(m, qp);
  
  m->marshal();

  // Qt could have changed it, and need to copy back to R
  if (qp && m->itemIsMutable()) 
    m->setSexp(to_sexp(qv, t));
}

template <typename T>
void marshal_to_sexp(MethodCall *m)
{
  T *qp;

  if (m->type().isPrimitive()) {
    m->setSexp(to_sexp(itemValue<T>(m), m->type()));
    return;
  }
    
  qp = (T*)m->item().s_voidp;

  if(!qp) {
    m->setSexp(R_NilValue);
    return;
  }
  m->setSexp(to_sexp(*qp, m->type()));
  
  m->marshal();
  
  if (m->itemIsMutable()) // R virtual could have changed it
    *qp = from_sexp<T>(m->sexp(), m->type());

  // Smoke allocates return values bigger than Stack
  if (m->returning() && !m->type().fitsStack()) 
    delete qp;
}

template <typename T>
static void marshal(MethodCall *m)
{
  switch(m->mode()) {
  case MethodCall::RToSmoke:
    marshal_from_sexp<T>(m);
    break;
 
  case MethodCall::SmokeToR:
    marshal_to_sexp<T>(m);
    break;
				
  default:
    m->unsupported();
    break;
  }
}

/* Special case the conversion of enums and objects */

/* Conversion of enums and classes requires extra data beyond the
   long/pointer that would be passed to and from the conversion
   routines. We could with some effort integrate these with the
   generic template; however, the benefits would be minimal, so we are
   leaving it how QtRuby left it. But it would be nice to
   factor out some of the logic for use when converting elements of
   non-scalars: collections, maps, etc. For this, we need to pass the
   type to the converters.  */

/* Dummy wrapper classes */

class SmokeClassWrapper {
public:
  MethodCall *m;
};

class SmokeEnumWrapper {
public:
  MethodCall *m;
};

/* enum marshaling */

template <>
void marshal_from_sexp<SmokeEnumWrapper>(MethodCall *m)
{
  SEXP v = m->sexp();
  setItemValue(m, enum_from_sexp<long>(v, m->type()));
}

template <>
void marshal_to_sexp<SmokeEnumWrapper>(MethodCall *m)
{
  long val = itemValue<long>(m);
  m->setSexp(enum_to_sexp(val, m->type()));
}

/* instance marshaling */

template <>
void marshal_from_sexp<SmokeClassWrapper>(MethodCall *m)
{
  SEXP v = m->sexp();
  
  SmokeObject *o = from_sexp<SmokeObject *>(v, m->type());
  
  if (o && m->returning() && !m->type().fitsStack()) {
    o = o->clone(); // Smoke takes ownership of virtual returns on the stack
  }

  void *ptr = o ? o->castPtr(m->type().className()) : NULL;
  setItemValue(m, ptr);
  
  return;
}

template <>
void marshal_to_sexp<SmokeClassWrapper>(MethodCall *m)
{
  void *p = itemValue<void *>(m);
  m->setSexp(ptr_to_sexp(p, m->type()));
}

/* Macros for initializing TypeHandler structures in an array. */  

#define TYPE_HANDLER_ENTRY_FULL(Alias, Type)    \
  { #Alias, marshal<Type>, scoreArg<Type> }

#define TYPE_HANDLER_ENTRY(Type)                \
  TYPE_HANDLER_ENTRY_FULL(Type, Type)

#define TYPE_HANDLER_ENTRY_PRIM(Type)                   \
  TYPE_HANDLER_ENTRY_FULL(Type*, Type),                 \
    TYPE_HANDLER_ENTRY_FULL(Type&, Type),               \
    TYPE_HANDLER_ENTRY_FULL(const Type&, Type),         \
    TYPE_HANDLER_ENTRY_FULL(const Type*, Type)

#define TYPE_HANDLER_ENTRY_INT(Type)                                    \
  TYPE_HANDLER_ENTRY_PRIM(Type),                                        \
    TYPE_HANDLER_ENTRY_FULL(const signed Type&, Type),                  \
    TYPE_HANDLER_ENTRY_FULL(const unsigned Type&, unsigned Type),       \
    TYPE_HANDLER_ENTRY_FULL(const signed Type*, Type),                  \
    TYPE_HANDLER_ENTRY_FULL(const unsigned Type*, unsigned Type),       \
    TYPE_HANDLER_ENTRY_FULL(signed Type*, Type),                        \
    TYPE_HANDLER_ENTRY_FULL(signed Type&, Type),                        \
    TYPE_HANDLER_ENTRY_FULL(unsigned Type*, unsigned Type),             \
    TYPE_HANDLER_ENTRY_FULL(unsigned Type*, unsigned Type)

#define TYPE_HANDLER_ENTRY_CLASS(Type)                  \
  TYPE_HANDLER_ENTRY(Type),                             \
    TYPE_HANDLER_ENTRY_FULL(const Type&, Type)

/* These exist to get around the problem of embedded commas in macro
   arguments. The only way to escape them is to wrap the argument in
   (), but that produces invalid C++ code.
*/
#define TYPE_HANDLER_ENTRY_FULL2(Alias, Alias2, Type, Type2)            \
  { #Alias "," #Alias2, marshal<Type,Type2>, scoreArg<Type,Type2> }

#define TYPE_HANDLER_ENTRY2(Type, Type2)                \
  TYPE_HANDLER_ENTRY_FULL2(Type, Type2, Type, Type2)

#define TYPE_HANDLER_ENTRY_CLASS2(Type, Type2)                  \
  TYPE_HANDLER_ENTRY2(Type, Type2),                             \
    TYPE_HANDLER_ENTRY_FULL2(const Type,Type2&, Type,Type2)

#define TYPE_HANDLER_ENTRY_FULL3(Alias, Alias2, Alias3, Type, Type2, Type3) \
  { #Alias "," #Alias2 "," #Alias3, marshal<Type,Type2,Type3>,          \
      scoreArg<Type,Type2,Type3> }

#define TYPE_HANDLER_ENTRY3(Type, Type2, Type3)                         \
  TYPE_HANDLER_ENTRY_FULL3(Type, Type2, Type3, Type, Type2, Type3)

#define TYPE_HANDLER_ENTRY_CLASS3(Type, Type2, Type3)                   \
  TYPE_HANDLER_ENTRY3(Type, Type2, Type3),                              \
    TYPE_HANDLER_ENTRY_FULL3(const Type,Type2,Type3&, Type,Type2,Type3)

#define TYPE_HANDLER_ENTRY_CLASS_ALL(Type)               \
  TYPE_HANDLER_ENTRY_CLASS(Type),                        \
    TYPE_HANDLER_ENTRY_FULL(const Type, Type),           \
    TYPE_HANDLER_ENTRY_FULL(Type*, Type),                \
    TYPE_HANDLER_ENTRY_FULL(const Type*, Type)

 
/*************************** ARGUMENT SCORING ************************/

int scoreArg_unknown(SEXP /*arg*/, const SmokeType &type);

template<typename T> int scoreArg(SEXP arg, const SmokeType &type) {
  return scoreArg_unknown(arg, type);
}

#endif
