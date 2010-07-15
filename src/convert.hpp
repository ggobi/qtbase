#ifndef CONVERT_H
#define CONVERT_H

#include <QVariant>
#include <QString>

// low-level conversion (reference wrapping)
#include "wrap.hpp" 

// for handling of classes and enums
#include "SmokeObject.hpp"
#include "SmokeType.hpp"

#undef isNull

/* Type conversion routines.
   
   Templates are picky about the type, so we need a lot of wrappers
   here to handle rare primitive types like 'short'. We avoid using
   the as<>()/wrap() functions from Rcpp, because the ones in R are
   better in some ways (they handle NA's, overflow, etc).

   The overloaded to_sexp() function is more forgiving.
 */

template <typename T> inline T from_sexp(SEXP v) {
}

// signed char

template <> inline
signed char from_sexp<signed char>(SEXP v)
{
  const char *str = CHAR(asChar(v));
  if (strlen(str))
    return str[0];
  else return 0;
}

inline SEXP to_sexp(signed char sv)
{
  char str[2];
  str[0] = sv;
  str[1] = '\0';
  return mkString(str);
}

// unsigned char

template <> inline 
unsigned char from_sexp<unsigned char>(SEXP v)
{
  if (!length(v)) {
    return 0;
  } else {
    return RAW(coerceVector(v, RAWSXP))[0];
  }
}

inline SEXP to_sexp(unsigned char sv)
{
  return ScalarRaw(sv);
}

// char

template <> inline
char from_sexp<char>(SEXP v)
{
  return from_sexp<signed char>(v);
}

// bool

template <> inline 
bool from_sexp<bool>(SEXP v)
{
  return asLogical(v);
}

inline SEXP to_sexp(bool sv)
{
  return ScalarLogical(sv);
}

// int

template <> inline 
int from_sexp<int>(SEXP v)
{
  return asInteger(v);
}

inline SEXP to_sexp(int sv) {
  return ScalarInteger(sv);
}

// short

template <> inline
short from_sexp<short>(SEXP v)
{
  return from_sexp<int>(v);
}

// unsigned short

template <> inline
unsigned short from_sexp<unsigned short>(SEXP v)
{
  return from_sexp<int>(v);
}


// double

template <> inline
double from_sexp<double>(SEXP v)
{
  return asReal(v);
}

SEXP inline to_sexp(double sv)
{
  return ScalarReal(sv);
}

// unsigned int

template <> inline
unsigned int from_sexp<unsigned int>(SEXP v)
{
  return from_sexp<double>(v);
}

inline SEXP to_sexp(unsigned int sv)
{
  return ScalarReal(sv);
}

// long

template <> inline
long from_sexp<long>(SEXP v)
{
  return from_sexp<int>(v);
}

inline SEXP to_sexp(long sv)
{
  return to_sexp((int)sv);
}

// unsigned long

template <> inline
unsigned long from_sexp<unsigned long>(SEXP v)
{
  return from_sexp<double>(v);
}

inline SEXP to_sexp(unsigned long sv)
{
  return to_sexp((unsigned int)sv);
}

// float

template <> inline
float from_sexp<float>(SEXP v)
{
  return from_sexp<double>(v);
}

// long long

template <> inline
long long from_sexp<long long>(SEXP v)
{
  return from_sexp<double>(v);
}

inline SEXP to_sexp(long long sv)
{
  return to_sexp((double)sv);
}

// unsigned long long

template <> inline
unsigned long long from_sexp<unsigned long long>(SEXP v)
{
  return from_sexp<double>(v);
}

inline SEXP to_sexp(unsigned long long sv)
{
  return to_sexp((double)sv);
}

// void*, just an opaque pointer

template<> inline void* from_sexp<void*>(SEXP sexp) {
  return sexp == R_NilValue ? NULL : R_ExternalPtrAddr(sexp);
}

inline SEXP to_sexp(void *ptr) {
  return R_MakeExternalPtr(ptr, R_NilValue, R_NilValue);
}

// QString

template<> inline QString from_sexp<QString>(SEXP s) {
  if (!length(s))
    return QString();
  return QString::fromUtf8(translateCharUTF8(asChar(s)));
}

inline SEXP to_sexp(QString s) {
  if (s.isNull())
    return R_NilValue;
  return ScalarString(mkChar(s.toLocal8Bit().data()));
}

/* NOTE: overloaded functions and template functions cannot be shared
   with other packages. Thus, we define these wrappers for the common
   case of QString conversion. */

inline QString sexp2qstring(SEXP s) { return from_sexp<QString>(s); }
inline SEXP qstring2sexp(QString s) { return to_sexp(s); }

/* Type conversion helpers that utilize Smoke type information */

/* so we do not need to add a type parameter to every converter */

template<typename T> inline T from_sexp(SEXP sexp, const SmokeType &type) {
  Q_UNUSED(type);
  return from_sexp<T>(sexp);
}
template<typename T> inline SEXP to_sexp(T value, const SmokeType &type) {
  Q_UNUSED(type);
  return to_sexp(value);
}

/* enum special case, no way to template just for enums */

template<typename T> inline T
enum_from_sexp(SEXP sexp, const SmokeType &type) {
  // FIXME: we could check validity here
  Q_UNUSED(type);
  return (T)from_sexp<long>(sexp);
}
template<typename T> inline SEXP
enum_to_sexp(T value, const SmokeType &type) {
  SEXP e, classes;
  PROTECT(e = ScalarInteger((long)value));
  classes = allocVector(STRSXP, 2);
  setAttrib(e, R_ClassSymbol, classes);
  SET_STRING_ELT(classes, 0, mkChar(type.name()));
  SET_STRING_ELT(classes, 1, mkChar("QtEnum"));
  UNPROTECT(1);
  return e;
}

/* class special cases, no way to template pointer/non-pointer */

// FIXME: find a way to more gracefully recover from a bad type
template<> inline SmokeObject*
from_sexp<SmokeObject*>(SEXP sexp, const SmokeType &type) {
  if (sexp == R_NilValue)
    return(NULL);
  SmokeObject * so = SmokeObject::fromSexp(sexp);  
  if (!so->instanceOf(type.className()))
    error("Expected an instance of type '%s', not '%s'", type.className(),
          so->className());
  return so;
}

// FIXME: need to check whether 'type' is valid for 'sexp'. Also, try
// to leverage QVariant to perform implicit conversions.

template<typename T> inline T /* special template for class pointers */
ptr_from_sexp(SEXP sexp, const SmokeType &type) {
  SmokeObject *so = from_sexp<SmokeObject *>(sexp, type);
  return (T)so->castPtr(type.className());
}

template<typename T> inline T /* for class pointers that need dereferencing */
class_from_sexp(SEXP sexp,  const SmokeType &type) {
  if (sexp == R_NilValue)
    error("'NULL' not allowed for non-pointer class types");
  return *(ptr_from_sexp<T*>(sexp, type));
}

/* Smoke passes objects via void pointer, which can represent an
   actual pointer, a reference or an object on the stack. All of
   these types can be qualified by 'const'.

   On the stack: Smoke allocates these and thus we need to free. We
   ignore the 'const' variant; it is very rare anyway.

   References: We need to copy the 'const' variant, and free it later.

   Pointers: We consider any non-const pointer to be allocated to R,
   even though objects are often owned by Qt. We try to catch this
   when the object falls out of R scope, but this only works for
   certain objects, like QObjects and QGraphicsItems. We ignore
   'const' pointers, as it usually does not make sense to copy them
   (unlike references), and they are somewhat rare.
*/

inline SEXP ptr_to_sexp(void *value, const SmokeType &type) {
  if (!value)
    return(R_NilValue);
  return SmokeObject::sexpFromPtr(value, type, !type.isConst(),
                                  type.isConst() && type.isRef());
}

template<typename T> inline SEXP
class_to_sexp(T value, const SmokeType &type) {
  return SmokeObject::sexpFromPtr(&value, type, false, true);
}

/* More special cases, at least partially implicit */

SEXP to_sexp(QVariant variant); /* <-> "any" R object */
QVariant qvariant_from_sexp(SEXP rvalue, int index = -1);
template<> inline QVariant from_sexp<QVariant>(SEXP rvalue) {
  return qvariant_from_sexp(rvalue);
}
QVariant asQVariantOfType(SEXP rvalue, QMetaType::Type type,
                          bool tryDirect = true);
template<> QList<QVariant> from_sexp<QList<QVariant> >(SEXP s,
                                                       const SmokeType &type);

/* QByteArray <-> raw or character vector */
template<> QByteArray from_sexp<QByteArray>(SEXP sexp, const SmokeType &type);
SEXP to_sexp(QByteArray s);

/* QString <-> character vector */
SEXP to_sexp(QList<QString> list);
template<> QList<QString> from_sexp<QList<QString> >(SEXP list);

template<> QStringList from_sexp<QStringList>(SEXP vector);

/* Explicit coercion */

template<> QRectF from_sexp<QRectF>(SEXP r); /* <-> 2x2 matrix */
template<> QRect from_sexp<QRect>(SEXP r);
SEXP to_sexp(QRectF rect);
SEXP to_sexp(QRect rect);

template<> QPointF from_sexp<QPointF>(SEXP p); /* <-> 2-vector */
template<> QPoint from_sexp<QPoint>(SEXP p);
SEXP to_sexp(QPointF point);
SEXP to_sexp(QPoint point);

template<> QSizeF from_sexp<QSizeF>(SEXP s); /* <-> 2-vector */
template<> QSize from_sexp<QSize>(SEXP s);
SEXP to_sexp(QSizeF size);
SEXP to_sexp(QSize size);

template<> QTransform from_sexp<QTransform>(SEXP m); /* <-> 3x3 matrix */
SEXP to_sexp(QTransform tform);
  
template<> QColor from_sexp<QColor>(SEXP c); /* <-> 4x1 matrix */
SEXP to_sexp(QColor color);

/* .Call entry points for explicit coercion */

#define DECL_COERCE_ENTRY_POINT(type)            \
  SEXP qt_coerce_##type(SEXP sexp)

DECL_COERCE_ENTRY_POINT(QRectF);
DECL_COERCE_ENTRY_POINT(QRect);
DECL_COERCE_ENTRY_POINT(QTransform);
DECL_COERCE_ENTRY_POINT(QPointF);
DECL_COERCE_ENTRY_POINT(QPoint);
DECL_COERCE_ENTRY_POINT(QPolygonF);
DECL_COERCE_ENTRY_POINT(QPolygon);
DECL_COERCE_ENTRY_POINT(QSizeF);
DECL_COERCE_ENTRY_POINT(QSize);
DECL_COERCE_ENTRY_POINT(QColor);

/* Conversion of non-scalars (collection, map, pair, ...) */

#define value_to_sexp to_sexp
#define value_from_sexp from_sexp

#define DEF_COLLECTION_CONVERTERS(Q, T, M)                              \
  SEXP to_sexp(Q<T> coll, const SmokeType &type) {                      \
    SEXP list;                                                          \
    PROTECT(list = allocVector(VECSXP, coll.size()));                   \
    SmokeType elementType(type.smoke(), #T);                            \
    int j = 0;                                                          \
    for(Q<T>::const_iterator i = coll.begin(); i != coll.end(); ++i, ++j ) \
      SET_VECTOR_ELT(list, j, M##_to_sexp(*i, elementType));            \
    UNPROTECT(1);                                                       \
    return list;                                                        \
  }                                                                     \
  template<> Q<T> from_sexp<Q<T> >(SEXP sexp, const SmokeType &type) {  \
    int count = length(sexp);                                           \
    Q<T> coll;                                                          \
    int i;                                                              \
    SmokeType elementType(type.smoke(), #T);                            \
    for(i = 0; i < count; i++)                                          \
      coll << M##_from_sexp<T>(VECTOR_ELT(sexp, i), elementType);       \
    return coll;                                                        \
  }

#define DEF_COLLECTION_CONVERTERS_PAIR(Q, T, U, M)                      \
  SEXP to_sexp(Q<QPair<T,U> > coll, const SmokeType &type) {            \
    SEXP list;                                                          \
    PROTECT(list = allocVector(VECSXP, coll.size()));                   \
    SmokeType elementType(type.smoke(), #Q "<QPair<" #T "," #U "> >");  \
    int j = 0;                                                          \
    for(Q<QPair<T,U> >::const_iterator i = coll.begin(); i != coll.end(); ++i) \
      SET_VECTOR_ELT(list, j++, M##_to_sexp(*i, elementType));          \
    UNPROTECT(1);                                                       \
    return list;                                                        \
  }                                                                     \
  template<> Q<QPair<T,U> >                                             \
  from_sexp<Q<QPair<T,U> > >(SEXP sexp, const SmokeType &type)          \
  {                                                                     \
    int count = length(sexp);                                           \
    Q<QPair<T,U> > coll;                                                \
    int i;                                                              \
    SmokeType elementType(type.smoke(), #Q "<QPair<" #T "," #U "> >");  \
    for(i = 0; i < count; i++)                                          \
      coll << M##_from_sexp<QPair<T,U> >(VECTOR_ELT(sexp, i), elementType); \
    return coll;                                                        \
  }

#define DEF_MAP_CONVERTERS(Q, K, V)                                     \
  SEXP to_sexp(Q<K,V> map, const SmokeType &type) {                     \
    SEXP list;                                                          \
    const char *names[] = { "key", "value" };                           \
    PROTECT(list = mkNamed(2, names));                                  \
    SmokeType keyType(type.smoke(), "QList<" #K ">");                   \
    SmokeType valueType(type.smoke(), "QList<" #V ">");                 \
    SET_VECTOR_ELT(list, 0, to_sexp(map.keys(), keyType));              \
    SET_VECTOR_ELT(list, 1, to_sexp(map.values(), valueType));          \
    UNPROTECT(1);                                                       \
    return list;                                                        \
  }                                                                     \
  template<> Q<K,V> from_sexp<Q<K,V> >(SEXP list, const SmokeType &type) { \
    Q<K, V> map;                                                        \
    SmokeType keyType(type.smoke(), "QList<" #K ">");                   \
    SmokeType valueType(type.smoke(), "QList<" #V ">");                 \
    QList<K> keys = from_sexp<QList<K> >(VECTOR_ELT(list, 0), keyType); \
    QList<V> values = from_sexp<QList<V> >(VECTOR_ELT(list, 1), valueType); \
    for (int i = 0; i < keys.size(); i++)                               \
      map[keys[i]] = values[i];                                         \
    return map;                                                         \
  }

#define DEF_PAIR_CONVERTERS(A, B, M, N)                                 \
  SEXP to_sexp(QPair<A,B> pair, const SmokeType &type) {                \
    SEXP list;                                                          \
    const char *names[] = { "first", "second" };                        \
    PROTECT(list = mkNamed(2, names));                                  \
    SmokeType firstType(type.smoke(), #A);                              \
    SmokeType secondType(type.smoke(), #B);                             \
    SET_VECTOR_ELT(list, 0, M##_to_sexp(pair.first, firstType));        \
    SET_VECTOR_ELT(list, 1, N##_to_sexp(pair.second, secondType));      \
    UNPROTECT(1);                                                       \
    return list;                                                        \
  }                                                                     \
  template<> QPair<A,B> from_sexp<QPair<A,B> >(SEXP list,const SmokeType &type)\
  {                                                                     \
    QPair<A, B> pair;                                                   \
    SmokeType firstType(type.smoke(), #A);                              \
    SmokeType secondType(type.smoke(), #B);                             \
    pair.first = M##_from_sexp<A>(VECTOR_ELT(list, 0), firstType);      \
    pair.second = N##_from_sexp<B>(VECTOR_ELT(list, 1), secondType);    \
    return pair;                                                        \
  }
    
#define DEF_PRIM_COLLECTION_CONVERTERS(Q, T, R)                         \
  SEXP to_sexp(Q<T> coll) {                                             \
    SEXP vector;                                                        \
    PROTECT(vector = NEW_##R(coll.size()));                             \
    for(int i = 0; i < length(vector); ++i )                            \
      R##_DATA(vector)[i] = coll.at(i);                                 \
    UNPROTECT(1);                                                       \
    return vector;                                                      \
  }                                                                     \
  template<> Q<T> from_sexp<Q<T> >(SEXP vector) {                       \
    Q<T> coll;                                                          \
    for(int i = 0; i < length(vector); i++)                             \
      coll.append(R##_DATA(vector)[i]);                                 \
    return coll;                                                        \
  }

#define DEF_MATRIX_CONVERTERS(N, M)                                     \
  SEXP to_sexp(QGenericMatrix<N,M,double> mat) {                        \
    SEXP rmat = allocMatrix(REALSXP, M, N);                             \
    const double *vals = mat.constData();                               \
    for (int i = 0; i < M; i++)                                         \
      for (int j = 0; j < N; i++)                                       \
        REAL(rmat)[j*M + i] = vals[i*N + j];                            \
    return rmat;                                                        \
  }                                                                     \
  template<> QGenericMatrix<N, M, double>                               \
  from_sexp<QGenericMatrix<N, M, double> >(SEXP rmat) {                 \
    double vals[M*N];                                             \
    for (int i = 0; i < M; i++)                                         \
      for (int j = 0; j < N; i++)                                       \
        vals[i*N + j] = REAL(rmat)[j*M + i];                            \
    return QGenericMatrix<N,M,double>(vals);                            \
  }

#define DEF_STRING_MAP_CONVERTERS(Q, V)                                 \
  SEXP to_sexp(Q<QString,V> map, const SmokeType &type) {               \
    SEXP keys, values;                                                  \
    SmokeType keyType(type.smoke(), "QList<QString>");                  \
    SmokeType valueType(type.smoke(), "QList<" #V ">");                 \
    PROTECT(values = to_sexp(map.values(), keyType));                   \
    keys = to_sexp(map.keys(), valueType);                              \
    setAttrib(values, R_NamesSymbol, keys);                             \
    UNPROTECT(1);                                                       \
    return values;                                                      \
  }                                                                     \
  template<> Q<QString,V>                                               \
  from_sexp<Q<QString,V> >(SEXP sexp, const SmokeType &type) {          \
    Q<QString, V> map;                                                  \
    SmokeType keyType(type.smoke(), "QList<QString>");                  \
    SmokeType valueType(type.smoke(), "QList<" #V ">");                 \
    QList<QString> keys =                                               \
      from_sexp<QList<QString> >(getAttrib(sexp, R_NamesSymbol), keyType); \
    QList<V> values = from_sexp<QList<V> >(sexp, valueType);             \
    for (int i = 0; i < keys.size(); i++)                               \
      map[keys[i]] = values[i];                                         \
    return map;                                                         \
  }

#endif
