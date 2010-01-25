
#ifndef QTBASE_C
#define QTBASE_C

#include <qtbase.h>
#include <R_ext/Rdynload.h>

/* Convenience macros from IRanges */

#define DEFINE_CCALLABLE_STUB(retT, stubname, Targs, args)              \
  typedef retT(*__ ## stubname ## _funtype__)Targs;                     \
  retT stubname Targs                                                   \
  {                                                                     \
   static __ ## stubname ## _funtype__ fun = NULL;                      \
   if (fun == NULL)                                                     \
     fun = (__ ## stubname ## _funtype__)                               \
       R_GetCCallable("qtbase", "_" #stubname);                         \
   return fun args;                                                     \
  }

/*
 * Using the above macro when retT (the returned type) is void will make Sun
 * Studio 12 C compiler unhappy. So we need to use the following macro to
 * handle that case.
 */
#define DEFINE_NOVALUE_CCALLABLE_STUB(stubname, Targs, args)            \
  typedef void(*__ ## stubname ## _funtype__)Targs;                     \
  void stubname Targs                                                   \
  {                                                                     \
   static __ ## stubname ## _funtype__ fun = NULL;                      \
   if (fun == NULL)                                                     \
     fun = (__ ## stubname ## _funtype__)                               \
       R_GetCCallable("qtbase", "_" #stubname);                         \
   fun args;                                                            \
   return;                                                              \
  }

/* Wrapping */

DEFINE_CCALLABLE_STUB(void*, _unwrapSmoke,
                      (SEXP x, const char *type),
                      (     x,             type))

DEFINE_CCALLABLE_STUB(SEXP, wrapPointer,
                      (void *ptr, QList<QString> classNames,
                       R_CFinalizer_t finalizer),
                      (      ptr,                classNames,
                                      finalizer))

/* Conversion */

DEFINE_CCALLABLE_STUB(QString, sexp2qstring, (SEXP s), (s))

DEFINE_CCALLABLE_STUB(const char **, asStringArray, (SEXP s_strs), (s_strs))

DEFINE_CCALLABLE_STUB(SEXP, asRStringArray, (const char * const * strs), (strs))

DEFINE_CCALLABLE_STUB(QRectF, asQRectF, (SEXP s), (s))

DEFINE_CCALLABLE_STUB(QPointF, asQPointF, (SEXP s), (s))

DEFINE_CCALLABLE_STUB(QSizeF, asQSizeF, (SEXP s), (s))

DEFINE_CCALLABLE_STUB(QMatrixF, asQMatrixF, (SEXP s), (s))

DEFINE_CCALLABLE_STUB(QColorF, asQColorF, (SEXP s), (s))

DEFINE_CCALLABLE_STUB(QColor *, asQColors, (SEXP s), (s))

DEFINE_CCALLABLE_STUB(QFont, asQFont, (SEXP s), (s))

DEFINE_CCALLABLE_STUB(SEXP, asRColor, (QColor s), (s))

DEFINE_CCALLABLE_STUB(SEXP, asRPointF, (QPointF s), (s))

DEFINE_CCALLABLE_STUB(SEXP, asRSizeF, (QSizeF s), (s))

DEFINE_CCALLABLE_STUB(SEXP, asRRectF, (QRectF s), (s))

DEFINE_CCALLABLE_STUB(SEXP, asRMatrix, (QMatrix s), (s))

DEFINE_CCALLABLE_STUB(SEXP, asRFont, (QFont s), (s))

DEFINE_CCALLABLE_STUB(SEXP, asRFont, (QFont s), (s))

/* Smoke module registration */

DEFINE_NOVALUE_CCALLABLE_STUB(registerRQtModule, (Smoke *s), (s))

#endif
