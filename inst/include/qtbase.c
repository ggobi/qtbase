
#ifndef QTBASE_C
#define QTBASE_C

#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>


SEXP
wrapQWidget(QWidget *widget)
{
    static SEXP(*fun)
        (QWidget*) = NULL;
    if (fun == NULL)
        fun = (SEXP(*)(QWidget*))
	    R_GetCCallable("qtbase", "wrapQWidget");
    return fun(widget);
}

SEXP
wrapQObject(QObject *object)
{
    static SEXP(*fun)
        (QObject*) = NULL;
    if (fun == NULL)
        fun = (SEXP(*)(QObject*))
	    R_GetCCallable("qtbase", "wrapQObject");
    return fun(object);
}


SEXP
wrapPointer(void *ptr, const char *className, R_CFinalizer_t finalizer)
{
    static SEXP(*fun)
        (void*, const char*, R_CFinalizer_t) = NULL;
    if (fun == NULL)
        fun = (SEXP(*)(void*, const char*, R_CFinalizer_t))
	    R_GetCCallable("qtbase", "wrapPointer");
    return fun(ptr, className, finalizer);
}


QObject*
unwrapQObjectReferee(SEXP x)
{
    static QObject*(*fun)
        (SEXP) = NULL;
    if (fun == NULL)
        fun = (QObject*(*)(SEXP))
	    R_GetCCallable("qtbase", "unwrapQObjectReferee");
    return fun(x);
}


#endif
