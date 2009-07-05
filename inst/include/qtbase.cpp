
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
wrapPointer(void *ptr, QList<QString> classNames, R_CFinalizer_t finalizer)
{
    static SEXP(*fun)
      (void*, QList<QString>, R_CFinalizer_t) = NULL;
    if (fun == NULL)
      fun = (SEXP(*)(void*, QList<QString>, R_CFinalizer_t))
	    R_GetCCallable("qtbase", "wrapPointer");
    return fun(ptr, classNames, finalizer);
}

SEXP wrapQGraphicsItem(QGraphicsItem *item, QList<QString> classNames)
{
  static SEXP(*fun)(QGraphicsItem*,QList<QString>) = NULL;
  if (fun == NULL)
    fun = (SEXP(*)(QGraphicsItem*,QList<QString>))
      R_GetCCallable("qtbase", "wrapQGraphicsItem");
  return fun(item, classNames);
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

QGraphicsItem*
unwrapQGraphicsItemReferee(SEXP x)
{
  static QGraphicsItem*(*fun)
    (SEXP) = NULL;
  if (fun == NULL)
    fun = (QGraphicsItem*(*)(SEXP))
      R_GetCCallable("qtbase", "unwrapQGraphicsItemReferee");
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


QRectF asQRectF(SEXP s)
{
  static QRectF(*fun)(SEXP) = NULL;
  if (fun == NULL)
    fun = (QRectF(*)(SEXP))R_GetCCallable("qtbase", "asQRectF");
  return fun(s);
}

QPointF asQPointF(SEXP s)
{
  static QPointF(*fun)(SEXP) = NULL;
  if (fun == NULL)
    fun = (QPointF(*)(SEXP))R_GetCCallable("qtbase", "asQPointF");
  return fun(s);
}

QSizeF asQSizeF(SEXP s)
{
  static QSizeF(*fun)(SEXP) = NULL;
  if (fun == NULL)
    fun = (QSizeF(*)(SEXP))R_GetCCallable("qtbase", "asQSizeF");
  return fun(s);
}

QMatrix asQMatrix(SEXP s)
{
  static QMatrix(*fun)(SEXP) = NULL;
  if (fun == NULL)
    fun = (QMatrix(*)(SEXP))R_GetCCallable("qtbase", "asQMatrix");
  return fun(s);
}

QColor asQColor(SEXP s)
{
  static QColor(*fun)(SEXP) = NULL;
  if (fun == NULL)
    fun = (QColor(*)(SEXP))R_GetCCallable("qtbase", "asQColor");
  return fun(s);
}

QColor *asQColors(SEXP s)
{
  static QColor*(*fun)(SEXP) = NULL;
  if (fun == NULL)
    fun = (QColor*(*)(SEXP))R_GetCCallable("qtbase", "asQColors");
  return fun(s);
}

QFont asQFont(SEXP s)
{
  static QFont(*fun)(SEXP) = NULL;
  if (fun == NULL)
    fun = (QFont(*)(SEXP))R_GetCCallable("qtbase", "asQFont");
  return fun(s);
}

SEXP asRColor(QColor s)
{
  static SEXP(*fun)(QColor) = NULL;
  if (fun == NULL)
    fun = (SEXP(*)(QColor))R_GetCCallable("qtbase", "asRColor");
  return fun(s);
}

SEXP asRPointF(QPointF s)
{
  static SEXP(*fun)(QPointF) = NULL;
  if (fun == NULL)
    fun = (SEXP(*)(QPointF))R_GetCCallable("qtbase", "asRPointF");
  return fun(s);
}

SEXP asRSizeF(QSizeF s)
{
  static SEXP(*fun)(QSizeF) = NULL;
  if (fun == NULL)
    fun = (SEXP(*)(QSizeF))R_GetCCallable("qtbase", "asRSizeF");
  return fun(s);
}

SEXP asRRectF(QRectF s)
{
  static SEXP(*fun)(QRectF) = NULL;
  if (fun == NULL)
    fun = (SEXP(*)(QRectF))R_GetCCallable("qtbase", "asRRectF");
  return fun(s);
}

SEXP asRMatrix(QMatrix s, bool inverted)
{
  static SEXP(*fun)(QMatrix, bool) = NULL;
  if (fun == NULL)
    fun = (SEXP(*)(QMatrix, bool))R_GetCCallable("qtbase", "asRMatrix");
  return fun(s, inverted);
}

SEXP asRFont(QFont s)
{
  static SEXP(*fun)(QFont) = NULL;
  if (fun == NULL)
    fun = (SEXP(*)(QFont))R_GetCCallable("qtbase", "asRFont");
  return fun(s);
}

/* Reference creation */

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

void addQGraphicsItemReference(QGraphicsItem *referee, QObject *referer) {
  static void (*fun)(QGraphicsItem *, QObject *) = NULL;
  if (fun == NULL)
    fun = (void(*)(QGraphicsItem *, QObject *))
      R_GetCCallable("qtbase", "addQGraphicsItemReference");
  fun(referee, referer);
}

#endif
