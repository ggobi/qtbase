
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
       R_GetCCallable("qtbase", #stubname);                             \
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
       R_GetCCallable("qtbase", #stubname);                             \
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

DEFINE_CCALLABLE_STUB(SEXP, _wrapSmoke,
                      (void *ptr, const char *className, bool allocated),
                      (      ptr,             className,      allocated))

/* Conversion */

DEFINE_CCALLABLE_STUB(QString, sexp2qstring, (SEXP s), (s))
DEFINE_CCALLABLE_STUB(SEXP, qstring2sexp, (QString s), (s))

/* Smoke module registration */

DEFINE_NOVALUE_CCALLABLE_STUB(registerRQtModule, (Smoke *s), (s))

#endif
