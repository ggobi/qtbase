
#ifndef QTBASE_C
#define QTBASE_C

#include <qtbase.h>
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

SEXP wrapQGraphicsWidget(QGraphicsWidget *widget)
{
  static SEXP(*fun)(QGraphicsWidget*) = NULL;
  if (fun == NULL)
    fun = (SEXP(*)(QGraphicsWidget*))
      R_GetCCallable("qtbase", "wrapQGraphicsWidget");
  return fun(widget);
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

QString sexp2qstring(SEXP s)
{
  static QString(*fun)(SEXP) = NULL;
  if (fun == NULL)
    fun = (QString(*)(SEXP))R_GetCCallable("qtbase", "sexp2qstring");
  return fun(s);
}

SEXP qstring2sexp(QString s)
{
  static SEXP(*fun)(QString) = NULL;
  if (fun == NULL)
    fun = (SEXP(*)(QString))R_GetCCallable("qtbase", "qstring2sexp");
  return fun(s);
}

const char ** asStringArray(SEXP s_strs) {
  static const char** (*fun)(SEXP) = NULL;
  if (fun == NULL)
    fun = (const char **(*)(SEXP))R_GetCCallable("qtbase", "asStringArray");
  return fun(s_strs);
}

SEXP asRStringArray(const char * const * strs) {
  static SEXP (*fun)(const char * const *) = NULL;
  if (fun == NULL)
    fun = (SEXP(*)(const char * const *))
      R_GetCCallable("qtbase", "asRStringArray");
  return fun(strs);
}

void addQObjectReference(QObject *referee, QObject *referer) {
  static void (*fun)(QObject *, QObject *) = NULL;
  if (fun == NULL)
    fun = (void(*)(QObject *, QObject *))
      R_GetCCallable("qtbase", "addQObjectReference");
  fun(referee, referer);
}

void addQWidgetReference(QWidget *referee, QObject *referer) {
  static void (*fun)(QWidget *, QObject *) = NULL;
  if (fun == NULL)
    fun = (void(*)(QWidget *, QObject *))
      R_GetCCallable("qtbase", "addQWidgetReference");
  fun(referee, referer);
}

void addQGraphicsWidgetReference(QGraphicsWidget *referee, QObject *referer) {
  static void (*fun)(QGraphicsWidget *, QObject *) = NULL;
  if (fun == NULL)
    fun = (void(*)(QGraphicsWidget *, QObject *))
      R_GetCCallable("qtbase", "addQGraphicsWidgetReference");
  fun(referee, referer);
}

#endif
