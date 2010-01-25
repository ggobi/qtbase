#ifndef TYPE_HANDLERS_H
#define TYPE_HANDLERS_H

#include "MethodCall.hpp"
#include "SmokeObject.hpp"

#include "convert.hpp"

template <class T> T* smoke_ptr(MethodCall *m) {
  return (T*) m->item().s_voidp;
}

template<> bool* smoke_ptr<bool>(MethodCall *m) { return &m->item().s_bool; }
template<> signed char* smoke_ptr<signed char>(MethodCall *m) {
  return &m->item().s_char;
}
template<> unsigned char* smoke_ptr<unsigned char>(MethodCall *m) {
  return &m->item().s_uchar;
}
template<> short* smoke_ptr<short>(MethodCall *m) { return &m->item().s_short; }
template<> unsigned short* smoke_ptr<unsigned short>(MethodCall *m) {
  return &m->item().s_ushort;
}
template<> int* smoke_ptr<int>(MethodCall *m) { return &m->item().s_int; }
template<> unsigned int* smoke_ptr<unsigned int>(MethodCall *m) {
  return &m->item().s_uint;
}
template<> long* smoke_ptr<long>(MethodCall *m) {
  return &m->item().s_long;
}
template<> unsigned long* smoke_ptr<unsigned long>(MethodCall *m) {
  return &m->item().s_ulong;
}
template<> float* smoke_ptr<float>(MethodCall *m) { return &m->item().s_float; }
template<> double* smoke_ptr<double>(MethodCall *m) {
  return &m->item().s_double;
}
template<> void* smoke_ptr<void>(MethodCall *m) { return m->item().s_voidp; }

template <class T> T sexp_to_primitive(SEXP);
template <class T> SEXP primitive_to_sexp(T);

template <class T> 
static void marshal_from_sexp(MethodCall *m) 
{
  SEXP obj = m->sexp();
  (*smoke_ptr<T>(m)) = sexp_to_primitive<T>(obj);
}

template <class T>
static void marshal_to_sexp(MethodCall *m)
{
  m->setSexp(primitive_to_sexp<T>( *smoke_ptr<T>(m) )); 
}

/*********************** PRIMITIVE TYPES *************************/

template <>
bool sexp_to_primitive<bool>(SEXP v)
{
  return asLogical(v);
}

template <>
SEXP primitive_to_sexp<bool>(bool sv)
{
  return ScalarLogical(sv);
}

template <>
signed char sexp_to_primitive<signed char>(SEXP v)
{
  const char *str = CHAR(asChar(v));
  if (strlen(str))
    return str[0];
  else return 0;
}

template <>
SEXP primitive_to_sexp<signed char>(signed char sv)
{
  char str[2];
  str[0] = sv;
  str[1] = '\0';
  return mkString(str);
}

template <>
unsigned char sexp_to_primitive<unsigned char>(SEXP v)
{
  if (!length(v)) {
    return 0;
  } else {
    return RAW(coerceVector(v, RAWSXP))[0];
  }
}

template <>
SEXP primitive_to_sexp<unsigned char>(unsigned char sv)
{
  return ScalarRaw(sv);
}

template <>
short sexp_to_primitive<short>(SEXP v)
{
  return asInteger(v);
}

template <>
SEXP primitive_to_sexp<short>(short sv)
{
  return ScalarInteger(sv);
}

template <>
unsigned short sexp_to_primitive<unsigned short>(SEXP v)
{
  return asInteger(v);
}

template <>
SEXP primitive_to_sexp<unsigned short>(unsigned short sv)
{
  return ScalarInteger(sv);
}

template <>
int sexp_to_primitive<int>(SEXP v)
{
  return asInteger(v);
}

template <>
SEXP primitive_to_sexp<int>(int sv)
{
  return ScalarInteger(sv);
}

template <>
unsigned int sexp_to_primitive<unsigned int>(SEXP v)
{
  return asReal(v);
}

template <>
SEXP primitive_to_sexp<unsigned int>(unsigned int sv)
{
  return ScalarReal(sv);
}

template <>
long sexp_to_primitive<long>(SEXP v)
{
  return asInteger(v);
}

template <>
SEXP primitive_to_sexp<long>(long sv)
{
  return ScalarInteger(sv);
}

template <>
unsigned long sexp_to_primitive<unsigned long>(SEXP v)
{
  return asReal(v);
}

template <>
SEXP primitive_to_sexp<unsigned long>(unsigned long sv)
{
  return ScalarReal(sv);
}

template <>
long long sexp_to_primitive<long long>(SEXP v)
{
  return asReal(v);
}

template <>
SEXP primitive_to_sexp<long long>(long long sv)
{
  return ScalarReal(sv);
}

template <>
unsigned long long sexp_to_primitive<unsigned long long>(SEXP v)
{
  return asReal(v);
}

template <>
SEXP primitive_to_sexp<unsigned long long>(unsigned long long sv)
{
  return ScalarReal(sv);
}

template <>
float sexp_to_primitive<float>(SEXP v)
{
  return (float) asReal(v);
}

template <>
SEXP primitive_to_sexp<float>(float sv)
{
  return ScalarReal(sv);
}

template <>
double sexp_to_primitive<double>(SEXP v)
{
  return (double) asReal(v);
}

template <>
SEXP primitive_to_sexp<double>(double sv)
{
  return ScalarReal(sv);
}

/*
template <>
char* sexp_to_primitive<char *>(SEXP rv)
{
  return CHAR(asChar(rv));
}

template <>
unsigned char* sexp_to_primitive<unsigned char *>(SEXP rv)
{
  return RAW(rv);
}
*/

template <>
SEXP primitive_to_sexp<int*>(int* sv)
{
  if(!sv) {
    return R_NilValue;
  }
	
  return primitive_to_sexp<int>(*sv);
}

#if defined(Q_OS_WIN32) || defined(Q_OS_MAC32)
template <>
static WId sexp_to_primitive<WId>(SEXP v)
{
  return (WId) asInteger(v);
}

template <>
static SEXP primitive_to_sexp<WId>(WId sv)
{
  return ScalarInteger((unsigned long) sv);
}

template <>
static Q_PID sexp_to_primitive<Q_PID>(SEXP v)
{
  return (Q_PID) asReal(v);
}

template <>
static SEXP primitive_to_sexp<Q_PID>(Q_PID sv)
{
  return ScalarReal((unsigned long) sv);
}
#endif

template <>
void marshal_from_sexp<long long>(MethodCall *m) 
{
  SEXP obj = m->sexp();
  m->item().s_voidp = new long long;
  *(long long *)m->item().s_voidp = sexp_to_primitive<long long>(obj);
	
  m->marshal();
	
  if(m->cleanup()) {
    delete (long long int *) m->item().s_voidp;
  }	
}

template <>
void marshal_from_sexp<unsigned long long>(MethodCall *m) 
{
  SEXP obj = m->sexp();
  m->item().s_voidp = new unsigned long long;
  *(long long *)m->item().s_voidp = sexp_to_primitive<unsigned long long>(obj);

  m->marshal();
	
  if(m->cleanup()) {
    delete (long long int *) m->item().s_voidp;
  }	
}

template <>
void marshal_from_sexp<int *>(MethodCall *m) 
{
  SEXP rv = m->sexp();
  int *i = new int;
	
  *i = asInteger(rv);
  m->item().s_voidp = i;
  m->marshal();
  
  if(m->cleanup()) {
    delete i;
  } else {
    m->item().s_voidp = new int((int)asInteger(rv));
  }
}

template <>
void marshal_to_sexp<int *>(MethodCall *m)
{
  int *ip = (int*)m->item().s_voidp;
  if(!ip) {
    m->setSexp(ScalarInteger(NA_INTEGER));
    return;
  }
	
  m->setSexp(ScalarInteger(*ip));
  m->marshal();
  if(!m->type().isConst())
    *ip = asInteger(m->sexp());
}

template <>
void marshal_from_sexp<unsigned int *>(MethodCall *m) 
{
  SEXP rv = m->sexp();
  unsigned int *i = new unsigned int;
	
  *i = asReal(rv);
  m->item().s_voidp = i;
  m->marshal();
  
  if(m->cleanup()) {
    delete i;
  } else {
    m->item().s_voidp = new int((int)asReal(rv));
  }
}

template <>
void marshal_to_sexp<unsigned int *>(MethodCall *m)
{
  unsigned int *ip = (unsigned int*) m->item().s_voidp;
  if (ip == 0) {
    m->setSexp(ScalarReal(NA_REAL));
    return;
  }
	
  m->setSexp(ScalarReal(*ip));
  m->marshal();
  if(!m->type().isConst())
    *ip = asReal(m->sexp());
}

template <>
void marshal_from_sexp<bool *>(MethodCall *m) 
{
  SEXP rv = m->sexp();
  bool * b = new bool;

  
  *b = asLogical(rv);
  m->item().s_voidp = b;
  m->marshal();

  if(m->cleanup()) {
    delete b;
  }
}

template <>
void marshal_to_sexp<bool *>(MethodCall *m)
{
  bool *ip = (bool*)m->item().s_voidp;
  if(!ip) {
    m->setSexp(ScalarLogical(NA_LOGICAL));
    return;
  }
  m->setSexp(ScalarLogical(*ip));
  m->marshal();
  if(!m->type().isConst())
    *ip = asLogical(m->sexp());
}

template <> 
void marshal_from_sexp<char *>(MethodCall *m) 
{
  SEXP rv = m->sexp();
  const char *str = CHAR(asChar(rv));
  if (!m->type().isConst())
    m->item().s_voidp = qstrdup(str);
  else m->item().s_voidp = const_cast<char *>(str);
  m->marshal();
  if(m->cleanup()) {
    char *retstr = (char *)m->item().s_voidp;
    int len = 1;
    if (!m->type().isRef()) // otherwise just a pointer to a single character
      len = strlen(retstr);
    PROTECT(rv = allocVector(STRSXP, 1));
    SET_STRING_ELT(rv, 0, mkCharLen(retstr, len));
    m->setSexp(rv);
    UNPROTECT(1);
    delete[] retstr;
  }
}

template <>
void marshal_from_sexp<unsigned char *>(MethodCall *m)
{
  SEXP rv = m->sexp();
  const unsigned char *bytes = RAW(rv);
  if (!m->type().isConst()) {
    unsigned char *tmp_bytes = new unsigned char[length(rv)];
    memcpy(tmp_bytes, bytes, length(rv));
    m->item().s_voidp = tmp_bytes;
  } else m->item().s_voidp = const_cast<unsigned char *>(bytes);
  m->marshal();
  if(m->cleanup()) {
    unsigned char *retbytes = (unsigned char *)m->item().s_voidp;
    delete[] retbytes;
  }
}

template <>
void marshal_to_sexp<char *>(MethodCall *m)
{
  char *sv = (char*)m->item().s_voidp;
  SEXP obj;
  if(sv) {
    int len = 1;
    PROTECT(obj = allocVector(STRSXP, 1));
    if (!m->type().isRef())
      len = strlen(sv);
    obj = mkCharLen(sv, len);
  } else obj = ScalarString(NA_STRING);

  if(m->cleanup())
    delete[] sv;

  m->setSexp(obj);
}

template <>
void marshal_to_sexp<unsigned char *>(MethodCall *m)
{
  m->unsupported();
}

/********************** SPECIAL CASES ************************/

class SmokeEnumWrapper {
public:
  MethodCall *m;
};	

class SmokeClassWrapper {
public:
  MethodCall *m;
};

template <>
void marshal_from_sexp<SmokeEnumWrapper>(MethodCall *m)
{
  SEXP v = m->sexp();
  m->item().s_enum = (long) asInteger(v);
}

template <>
void marshal_to_sexp<SmokeEnumWrapper>(MethodCall *m)
{
  long val = m->item().s_enum;
  SEXP e, classes;
  PROTECT(e = ScalarInteger(val));
  classes = allocVector(STRSXP, 2);
  setAttrib(e, R_ClassSymbol, classes);
  SET_STRING_ELT(classes, 0, mkChar(m->type().name()));
  SET_STRING_ELT(classes, 1, mkChar("QtEnum"));
  m->setSexp(e);
  UNPROTECT(1);
}

extern QHash<QByteArray, TypeHandler*> type_handlers;

template <>
void marshal_from_sexp<SmokeClassWrapper>(MethodCall *m)
{
  SEXP v = m->sexp();

  /* special-case QVariant[*&], convert from R value */
  const char *name = m->type().name();
  if (!qstrncmp(name, "QVariant", 8) && strlen(name) <= 9) {
    QVariant *variant = new QVariant(asQVariant(v));
    m->item().s_class = variant;
    m->marshal();
    if (m->cleanup())
      delete variant;
    return;
  }
  
  if (v == R_NilValue) {
    m->item().s_class = 0;
    return;
  }

  SmokeObject *o = SmokeObject::fromSexp(v);		
  void *ptr = o->ptr();
  
  if (!m->cleanup() && m->type().isStack()) {
    o = o->clone();
#ifdef DEBUG
    qWarning("copying %s %p to %p\n", o->klass()->name(), o->ptr(), ptr);
#endif
  }
  
  const Smoke::Class &cl = m->smoke()->classes[m->type().classId()];
  ptr = o->castPtr(cl.className);
  
  m->item().s_class = ptr;
  return;
}

template <>
void marshal_to_sexp<SmokeClassWrapper>(MethodCall *m)
{
  void *p = m->item().s_voidp;
  
  /* special-case QVariant[*&], convert to R value */
  const char *name = m->type().name();
  if (!qstrncmp(name, "QVariant", 8) && strlen(name) <= 9) {
    m->setSexp(asRVariant(*reinterpret_cast<QVariant *>(p)));
    return;
  }
  
  if (p == 0) {
    m->setSexp(R_NilValue);
    return;
  }
  
  /* ML: We are handling pointers and references to classes.. These
     can be optionally be 'const'. We are also handling classes on the
     stack, and Smoke allocates these, so we need to free them.

     Non-const reference types are probably considered bad form and
     thus are rare. Const references are convenient in that they can
     be read without being copied but need to be copied before
     modification and before the memory disappears. To be safe, we
     always copy them.

     We consider any non-const pointer to be allocated to R, even
     though objects are often owned by Qt. We try to catch this when
     the object falls out of R scope, but this only works for certain
     objects, like QObjects and QGraphicsItems. Const pointers are
     somewhat rare (who wants an object they cannot modify?)
  */
  
  SmokeObject *o =
    SmokeObject::fromPtr(p, m->smoke(), m->type().classId(),
                         !m->type().isConst(),
                         m->type().isConst() && m->type().isRef());
  m->setSexp(o->sexp());
}

/******************************* MACROS ******************************/

#define DEF_HASH_MARSHALLER(HashIdent,Item) namespace { char HashIdent##STR[] = #Item; } \
  TypeHandler::MarshalFn marshal_##HashIdent = marshal_Hash<Item,HashIdent##STR>;

#define DEF_MAP_MARSHALLER(MapIdent,Item) namespace { char MapIdent##STR[] = #Item; } \
  TypeHandler::MarshalFn marshal_##MapIdent = marshal_Map<Item,MapIdent##STR>;

#define DEF_LIST_MARSHALLER(ListIdent,ItemList,Item) namespace { char ListIdent##STR[] = #Item; } \
  TypeHandler::MarshalFn marshal_##ListIdent = marshal_ItemList<Item,ItemList,ListIdent##STR>;

#define DEF_VALUELIST_MARSHALLER(ListIdent,ItemList,Item) namespace { char ListIdent##STR[] = #Item; } \
  TypeHandler::MarshalFn marshal_##ListIdent = marshal_ValueListItem<Item,ItemList,ListIdent##STR>;

#define DEF_LINKED_LIST_MARSHALLER(ListIdent,ItemList,Item) namespace { char ListIdent##STR[] = #Item; } \
  TypeHandler::MarshalFn marshal_##ListIdent = marshal_LinkedItemList<Item,ItemList,ListIdent##STR>;

#define DEF_LINKED_VALUELIST_MARSHALLER(ListIdent,ItemList,Item) namespace { char ListIdent##STR[] = #Item; } \
  TypeHandler::MarshalFn marshal_##ListIdent = marshal_LinkedValueListItem<Item,ItemList,ListIdent##STR>;

#define DEF_VALUE_MARSHALLER(Type)                                      \
  TypeHandler::MarshalFn marshal_Q##Type =                              \
    marshal_Value<Q##Type, asR##Type, asQ##Type>;

#define DEF_VALUE_SCORER(Type)                                          \
  TypeHandler::ScoreArgFn scoreArg_Q##Type = scoreArg_byClass;
//scoreArg_Value<canConvert##Type>; TODO after we have canConvert*

#define DEF_VALUE_MARSHALLER_SCORER(Type) \
  DEF_VALUE_MARSHALLER(Type)              \
  DEF_VALUE_SCORER(Type)

#define TYPE_HANDLER_ENTRY_LIST(Type)                            \
  { "QList<" #Type "*>", marshal_##Type##List, NULL },           \
  { "const QList<" #Type "*>&", marshal_##Type##List, NULL }  

#define TYPE_HANDLER_ENTRY_VECTOR(Type)                                \
  { "QVector<" #Type "*>", marshal_##Type##Vector, NULL },             \
  { "const QVector<" #Type "*>&", marshal_##Type##Vector, NULL }  

#define TYPE_HANDLER_ENTRY_VALUEVECTOR(Type)                           \
  { "QVector<" #Type ">", marshal_##Type##Vector, NULL },              \
  { "const QVector<" #Type ">&", marshal_##Type##Vector, NULL }  

#define TYPE_HANDLER_ENTRY_VALUELIST(Type)                              \
  { "QList<" #Type ">", marshal_##Type##List, NULL },              \
  { "const QList<" #Type ">&", marshal_##Type##List, NULL }  

#define TYPE_HANDLER_ENTRY_PAIRLIST(A, B)                               \
  { "QList<QPair<" #A "," #B "> >", marshal_QPair##A##B##List, NULL },  \
  { "const QList<QPair<" #A "," #B "> >&", marshal_QPair##A##B##List, NULL }  

#define TYPE_HANDLER_ENTRY_PAIRVECTOR(A, B)                                 \
  { "QVector<QPair<" #A "," #B "> >", marshal_QPair##A##B##Vector, NULL },  \
  { "const QVector<QPair<" #A "," #B "> >&", marshal_QPair##A##B##Vector, NULL }

#define TYPE_HANDLER_ENTRY_VALUE(Type)                      \
  { #Type, marshal_##Type, scoreArg_##Type },               \
  { "const " #Type "&", marshal_##Type, scoreArg_##Type },  \
  { #Type "*", marshal_##Type, scoreArg_##Type },           \
  { "const " #Type "*", marshal_##Type, scoreArg_##Type }

#define TYPE_HANDLER_ENTRY_MATRIX(N, M, Type)                       \
  { "const QGenericMatrix<" #N "," #M "," #Type ">",                \
      marshal_QGenericMatrix<N, M, Type>, NULL },                   \
  { "const QGenericMatrix<" #N "," #M "," #Type ">*",               \
      marshal_QGenericMatrix<N, M, Type>, NULL }

#define TYPE_HANDLER_ENTRY_MAP(A, B)                            \
  { "QMap<" #A "," #B ">", marshal_QMap##A##B, NULL },          \
  { "const QMap<" #A "," #B ">&", marshal_QMap##A##B, NULL }  

#define TYPE_HANDLER_ENTRY_PAIR(A, B)                             \
  { "QPair<" #A "," #B ">", marshal_QPair##A##B, NULL },          \
  { "const QPair<" #A "," #B ">&", marshal_QPair##A##B, NULL }  

template <class Type, SEXP (*toR)(Type), Type (*fromR)(SEXP)>
void marshal_Value(MethodCall *m) {
  switch(m->mode()) {
  case MethodCall::RToSmoke:
    { 
      Type *s = NULL;
      if(!m->type().isPtr() || m->sexp() != R_NilValue) {
        s = new Type(fromR(m->sexp()));
      }
      
      m->item().s_voidp = s;
      m->marshal();
      
      if (m->cleanup())
        delete s;
      
      if (!m->type().isStack() && !m->type().isConst() &&
          m->sexp() != R_NilValue)
        { 
          m->setSexp(toR(*s));
        }
    }
    break;
    
  case MethodCall::SmokeToR:
    {
      Type *s = (Type*)m->item().s_voidp;
      if(s) {
        m->setSexp(toR(*s));
        if(m->cleanup()) {
          delete s;
        }
      } else {
        m->setSexp(R_NilValue);
      }
    }
    break;
    
  default:
    m->unsupported();
    break;
  }
}

int scoreArg_byClass(SEXP arg, const SmokeType &type);
  
template <bool (*CanConvert)(SEXP)>
int scoreArg_Value(SEXP arg, const SmokeType &type) {
  int score = scoreArg_byClass(arg, type);
  if (!score)
    score = CanConvert(arg);
  return score;
}

template <class Item, class ItemList, const char *ItemSTR >
void marshal_ItemList(MethodCall *m) {
  switch(m->mode()) {
  case MethodCall::RToSmoke:
    {
      SEXP list = m->sexp();
      if (TYPEOF(list) != VECSXP) {
        m->item().s_voidp = 0;
        break;
      }

      int count = length(list);
      ItemList *cpplist = new ItemList;
      long i;
      for(i = 0; i < count; i++) {
        SEXP item = VECTOR_ELT(list, i);
        // TODO do type checking!
        SmokeObject *o = SmokeObject::fromSexp(item);
        if(!o || !o->ptr())
          continue;
        void *ptr = o->castPtr(ItemSTR);
        cpplist->append((Item*)ptr);
      }

      m->item().s_voidp = cpplist;
      m->marshal();
      
      if (!m->type().isConst()) {
        PROTECT(list = allocVector(VECSXP, count));
        for(int i = 0; i < cpplist->size(); ++i ) {
          SmokeObject *so =
            SmokeObject::fromPtr((void*)cpplist->at(i), m->smoke(),
                                 m->smoke()->idClass(ItemSTR).index);
          SEXP obj = so->sexp();
          SET_VECTOR_ELT(list, i, obj);
        }
        UNPROTECT(1);
      }
      m->setSexp(list);
      
      if (m->cleanup()) {
        delete cpplist;
      }
    }
    break;
      
  case MethodCall::SmokeToR:
    {
      ItemList * cpplist = (ItemList *) m->item().s_voidp;
      if (cpplist == 0) {
        m->setSexp(R_NilValue);
        break;
      }

      SEXP list;
      PROTECT(list = allocVector(VECSXP, cpplist->size()));
      for(int i = 0; i < length(list); ++i ) {
        SmokeObject *so =
          SmokeObject::fromPtr((void*)cpplist->at(i), m->smoke(),
                               m->smoke()->idClass(ItemSTR).index);
        SEXP obj = so->sexp();
        SET_VECTOR_ELT(list, i, obj);
      }
      UNPROTECT(1);
      
      m->setSexp(list);
      m->marshal();

      if (!m->type().isConst()) {
        int count = length(list);
        long i;
        cpplist->clear();
        for (i = 0; i < count; i++) {
          SEXP item = VECTOR_ELT(list, i);
          // TODO do type checking!
          SmokeObject *o = SmokeObject::fromSexp(item);
          if(!o || !o->ptr())
            continue;
          void *ptr = o->castPtr(ItemSTR);
          cpplist->append((Item*)ptr);
        }
      }

      if (m->cleanup()) {
        delete cpplist;
      }
    }
    break;

  default:
    m->unsupported();
    break;
  }
}

/* ML: This seems a bit strange. It produces a list of pointers to
   memory owned by the QList. If the QList is destroyed, we are in
   trouble.
*/
template <class Item, class ItemList, const char *ItemSTR >
void marshal_ValueListItem(MethodCall *m) {
  switch(m->mode()) {
  case MethodCall::RToSmoke:
    {
      SEXP list = m->sexp();
      if (TYPEOF(list) != VECSXP) {
        m->item().s_voidp = 0;
        break;
      }
      int count = length(list);
      ItemList *cpplist = new ItemList;
      long i;
      for(i = 0; i < count; i++) {
        SEXP item = VECTOR_ELT(list, i);
        // TODO do type checking!
        SmokeObject *o = SmokeObject::fromSexp(item);

        if (!o || !o->ptr())
          continue;
				
        void *ptr = o->castPtr(ItemSTR);
        cpplist->append(*(Item*)ptr);
      }

      m->item().s_voidp = cpplist;
      m->marshal();

      if (!m->type().isConst()) {
        PROTECT(list = allocVector(VECSXP, count));
        for(int i = 0; i < cpplist->size(); ++i ) {
          SmokeObject *so =
            SmokeObject::fromPtr((void*)&cpplist->at(i), m->smoke(),
                                 m->smoke()->idClass(ItemSTR).index);
          SEXP obj = so->sexp();
          SET_VECTOR_ELT(list, i, obj);
        }
        UNPROTECT(1);
      }
      m->setSexp(list);

      if (m->cleanup()) {
        delete cpplist;
      }
    }
    break;
      
  case MethodCall::SmokeToR:
    {
      ItemList *valuelist = (ItemList*)m->item().s_voidp;
      if(!valuelist) {
        m->setSexp(R_NilValue);
        break;
      }

      SEXP list;
      PROTECT(list = allocVector(VECSXP, valuelist->size()));
      for(int i = 0; i < length(list); ++i ) {
        SmokeObject *so =
          SmokeObject::fromPtr((void*)&(valuelist->at(i)), m->smoke(),
                               m->smoke()->idClass(ItemSTR).index);
        SEXP obj = so->sexp();
        SET_VECTOR_ELT(list, i, obj);
      }
      UNPROTECT(1);

      m->setSexp(list);
      m->marshal();

      if (m->cleanup()) {
        delete valuelist;
      }

    }
    break;
      
  default:
    m->unsupported();
    break;
  }
}

/*
  The code for the QLinkedList marshalers is identical to the QList
  and QVector marshallers apart from the use of iterators instead of
  at(), and so it really should be possible to code one marshaller to
  work with all three types.  */

template <class Item, class ItemList, const char *ItemSTR >
void marshal_LinkedItemList(MethodCall *m) {
  switch(m->mode()) {
  case MethodCall::RToSmoke:
    {
      SEXP list = m->sexp();
      if (TYPEOF(list) != VECSXP) {
        m->item().s_voidp = 0;
        break;
      }

      int count = length(list);
      ItemList *cpplist = new ItemList;
      long i;
      for (i = 0; i < count; i++) {
        SEXP item = VECTOR_ELT(list, i);
        // TODO do type checking!
        SmokeObject *o = SmokeObject::fromSexp(item);
        if (o == 0 || o->ptr() == 0)
          continue;
        void *ptr = o->castPtr(ItemSTR);
        cpplist->append((Item*)ptr);
      }

      
      m->item().s_voidp = cpplist;
      m->marshal();

      if (!m->type().isConst()) {
        int classId = m->smoke()->idClass(ItemSTR).index;
        PROTECT(list = allocVector(VECSXP, count));
        i = 0;
        QLinkedListIterator<Item*> iter(*cpplist);
        while (iter.hasNext()) {
          SmokeObject *so =
            SmokeObject::fromPtr((void *) iter.next(), m->smoke(), classId);
          SET_VECTOR_ELT(list, i++, so->sexp());
        }
        m->setSexp(list);
        UNPROTECT(1);   
      }

      if (m->cleanup()) {
        delete cpplist;
      }
    }
    break;
      
  case MethodCall::SmokeToR:
    {
      ItemList *valuelist = (ItemList*)m->item().s_voidp;
      if (valuelist == 0) {
        m->setSexp(R_NilValue);
        break;
      }

      int i = 0;
      SEXP av;
      PROTECT(av = allocVector(VECSXP, valuelist.size()));

      QLinkedListIterator<Item*> iter(*valuelist);
      while (iter.hasNext()) {
        void * p = (void *) iter.next();

        if (m->item().s_voidp == 0) {
          m->setSexp(R_NilValue);
          break;
        }

        SmokeObject *so =
          SmokeObject::fromPtr(p, m->smoke(),
                               m->smoke()->idClass(ItemSTR).index);
        SEXP obj = so->sexp();
        SET_VECTOR_ELT(av, i++, obj);
      }

      m->setSexp(av);
      m->marshal();

      if (m->cleanup()) {
        delete valuelist;
      }
      UNPROTECT(1);
    }
    break;

  default:
    m->unsupported();
    break;
  }
}

template <class Item, class ItemList, const char *ItemSTR >
void marshal_LinkedValueListItem(MethodCall *m) {
  switch(m->mode()) {
  case MethodCall::RToSmoke:
    {
      SEXP list = m->sexp();
      if (TYPEOF(list) != VECSXP) {
        m->item().s_voidp = 0;
        break;
      }
      int count = length(list);
      ItemList *cpplist = new ItemList;
      long i;
      for(i = 0; i < count; i++) {
        SEXP item = VECTOR_ELT(list, i);
        // TODO do type checking!
        SmokeObject *o = SmokeObject::fromSexp(item);
        
        if (o == 0 || o->ptr() == 0)
          continue;
				
        void *ptr = o->castPtr(ItemSTR);
        cpplist->append(*(Item*)ptr);
      }

      m->item().s_voidp = cpplist;
      m->marshal();

      PROTECT(list = allocVector(VECSXP, count));

      int classId = m->smoke()->idClass(ItemSTR).index;
      if (!m->type().isConst()) {
        i = 0;
        QLinkedListIterator<Item> iter(*cpplist);
        while (iter.hasNext()) {
          SmokeObject *so =
            SmokeObject::fromPtr((void*)&(iter.next()), m->smoke(), classId);
          SET_VECTOR_ELT(list, i++, so->sexp());
        }
      }

      if (m->cleanup()) {
        delete cpplist;
      }
      UNPROTECT(1);
    }
    break;
      
  case MethodCall::SmokeToR:
    {
      ItemList *valuelist = (ItemList*)m->item().s_voidp;
      if (valuelist == 0) {
        m->setSexp(R_NilValue);
        break;
      }

      SEXP av;
      PROTECT(av = allocVector(VECSXP, valuelist.size()));

      int ix = m->smoke()->idClass(ItemSTR).index;
      QLinkedListIterator<Item> iter(*valuelist);
      int i = 0;
      while (iter.hasNext()) {
        void * p = (void*) &(iter.next());

        if(m->item().s_voidp == 0) {
          m->setSexp(R_NilValue);
          break;
        }

        SmokeObject *so = SmokeObject::fromPtr(p, m->smoke(), ix);
        SET_VECTOR_ELT(av, i++, so->sexp());
      }

      m->setSexp(av);
      m->marshal();

      if (m->cleanup()) {
        delete valuelist;
      }
      UNPROTECT(1);
    }
    break;
      
  default:
    m->unsupported();
    break;
  }
}

template <class Value, const char *ValueSTR >
void marshal_Hash(MethodCall *m) {
  switch(m->mode()) {
  case MethodCall::RToSmoke:
    {
      SEXP hv = m->sexp();
      if (TYPEOF(hv) != VECSXP) {
        m->item().s_voidp = 0;
        break;
      }
		
      QHash<QString, Value*> * hash = new QHash<QString, Value*>;

      SEXP keys = getAttrib(hv, R_NamesSymbol);
      for (long i = 0; i < length(keys); i++) {
        SEXP key = STRING_ELT(keys, i);
        SEXP value = VECTOR_ELT(hv, i);
        
        SmokeObject *o = SmokeObject::fromSexp(value);
        if( !o || !o->ptr())
          continue;
        void *val_ptr = o->castPtr(ValueSTR);
        (*hash)[QString(CHAR(key))] = (Value*)val_ptr;
      }
	    
      m->item().s_voidp = hash;
      m->marshal();
		
      if (m->cleanup())
        delete hash;
    }
    break;
  case MethodCall::SmokeToR:
    {
      QHash<QString, Value*> *hash =
        (QHash<QString, Value*>*) m->item().s_voidp;
      if (hash == 0) {
        m->setSexp(R_NilValue);
        break;
      }
		
      SEXP hv, names;
      PROTECT(hv = allocVector(VECSXP, hash.size()));
      names = allocVector(STRSXP, length(hv));
      setAttrib(hv, R_NamesSymbol, names);
      
      int val_ix = m->smoke()->idClass(ValueSTR).index;
      
      int i = 0;
      for (QHashIterator<QString, Value*> it(*hash); it.hasNext(); it.next()) {
        void *val_p = it.value();
        SmokeObject *so = SmokeObject::fromPtr(val_p, m->smoke(), val_ix);
        SET_VECTOR_ELT(hv, i, so->sexp());
        SET_STRING_ELT(hv, i++, asChar(qstring2sexp(it.key())));
      }
      
      m->setSexp(hv);
      m->marshal();
		
      if (m->cleanup())
        delete hash;
      UNPROTECT(1);
    }
    break;
  default:
    m->unsupported();
    break;
  }
}

template <class Value, const char *ValueSTR >
void marshal_Map(MethodCall *m) {
  switch(m->mode()) {
  case MethodCall::RToSmoke:
    {
      SEXP hv = m->sexp();
      if (TYPEOF(hv) != VECSXP) {
        m->item().s_voidp = 0;
        break;
      }
		
      QMap<QString, Value> * map = new QMap<QString, Value>;
      
      SEXP keys = getAttrib(hv, R_NamesSymbol);
      for (long i = 0; i < length(keys); i++) {
        SEXP key = STRING_ELT(keys, i);
        SEXP value = VECTOR_ELT(hv, i);
        
        SmokeObject *o = SmokeObject::fromSexp(value);
        if( !o || !o->ptr())
          continue;
        void * val_ptr = o->castPtr(ValueSTR);
        (*map)[QString(CHAR(key))] = (Value*)val_ptr;
      }
      	    
      m->item().s_voidp = map;
      m->marshal();
		
      if (m->cleanup()) {
        delete map;
      }
    }
    break;
  case MethodCall::SmokeToR:
    {
      QMap<QString, Value> *map = (QMap<QString, Value>*) m->item().s_voidp;
      if (map == 0) {
        m->setSexp(R_NilValue);
        break;
      }

      SEXP hv, names;
      PROTECT(hv = allocVector(VECSXP, map.size()));
      names = allocVector(STRSXP, length(hv));
      setAttrib(hv, R_NamesSymbol, names);
      
      int val_ix = m->smoke()->idClass(ValueSTR).index;
      
      int i = 0;
      for (QHashIterator<QString, Value*> it(*map); it.hasNext(); it.next()) {
        void *val_p = it.value();
        SmokeObject *so = SmokeObject::fromPtr(val_p, m->smoke(), val_ix);
        SET_VECTOR_ELT(hv, i, so->sexp());
        SET_STRING_ELT(hv, i++, asChar(qstring2sexp(it.key())));
      }
      
      m->setSexp(hv);
      m->marshal();
		
      if (m->cleanup()) {
        delete map;
      }
      UNPROTECT(1);
    }
    break;
  default:
    m->unsupported();
    break;
  }
}
#endif
