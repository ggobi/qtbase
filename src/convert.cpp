#include "convert.hpp"

SEXP asRVariant(QVariant variant) {
  SEXP ans = NULL;
  switch(variant.type()) {
  case QMetaType::Void:
    break;
  case QMetaType::Bool:
    ans = ScalarLogical(variant.value<bool>());
    break;
  case QMetaType::Int:
  case QMetaType::UInt:
  case QMetaType::Long:
  case QMetaType::Short:
  case QMetaType::UShort:
    ans = ScalarInteger(variant.value<int>());
    break;
  case QMetaType::Double:
  case QMetaType::LongLong:
  case QMetaType::ULong:
  case QMetaType::ULongLong:
  case QMetaType::Float:
    ans = ScalarReal(variant.value<double>());
    break;
  case QMetaType::QChar:
  case QMetaType::Char:
  case QMetaType::UChar:
  case QMetaType::QString:
  case QMetaType::QByteArray:
    ans = qstring2sexp(variant.value<QString>());
    break;
  case QMetaType::VoidStar:
    ans = wrapPointer(variant.value<void *>());
    break;
  case QMetaType::QObjectStar:
    ans = wrapQObject(variant.value<QObject *>());
    break;
  case QMetaType::QWidgetStar:
    ans = wrapQWidget(variant.value<QWidget *>());
    break;
  case QMetaType::QCursor:
    break;
  case QMetaType::QDate:
    break;
  case QMetaType::QSize:
  case QMetaType::QSizeF:
    ans = asRSizeF(variant.value<QSizeF>());
    break;
  case QMetaType::QTime:
    break;
  case QMetaType::QVariantList:
    break;
  case QMetaType::QPolygon:
    break;
  case QMetaType::QColor:
    ans = asRColor(variant.value<QColor>());
    break;
  case QMetaType::QRectF:
  case QMetaType::QRect:
    ans = asRRectF(variant.value<QRectF>());
    break;
  case QMetaType::QLine:
    break;
  case QMetaType::QTextLength:
    break;
  case QMetaType::QStringList:
    break;
  case QMetaType::QVariantMap:
    break;
  case QMetaType::QVariantHash:
    break;
  case QMetaType::QIcon:
    break;
  case QMetaType::QPen:
    break;
  case QMetaType::QLineF:
    break;
  case QMetaType::QTextFormat:
    break;
  case QMetaType::QPoint:
  case QMetaType::QPointF:
    ans = asRPointF(variant.value<QPointF>());
    break;
  case QMetaType::QUrl:
    break;
  case QMetaType::QRegExp:
    break;
  case QMetaType::QDateTime:
    break;
  case QMetaType::QPalette:
    break;
  case QMetaType::QFont:
    ans = asRFont(variant.value<QFont>());
    break;
  case QMetaType::QBrush:
    break;
  case QMetaType::QRegion:
    break;
  case QMetaType::QBitArray:
    break;
  case QMetaType::QImage:
    break;
  case QMetaType::QKeySequence:
    break;
  case QMetaType::QSizePolicy:
    break;
  case QMetaType::QPixmap:
    break;
  case QMetaType::QLocale:
    break;
  case QMetaType::QBitmap:
    break;
  case QMetaType::QMatrix:
    ans = asRMatrix(variant.value<QMatrix>(), FALSE);
    break;
  case QMetaType::QTransform:
    break;
  case QMetaType::User:
    break;
  default:
    error("Converting from QVariant: unhandled Qt type");
  }
  if (!ans)
    error("Converting from QVariant: Qt type not yet implemented");
  return ans;
}

/* create a QVariant directly from an R object */
QVariant asQVariant(SEXP rvalue) {
  QVariant variant;
  switch(TYPEOF(rvalue)) {
  case LGLSXP:
    // Rprintf("Logical\n");
    variant = QVariant(asLogical(rvalue));
    break;
  case REALSXP:
    // Rprintf("Real\n");
    variant = QVariant(asReal(rvalue));
    break;
  case INTSXP:
    // Rprintf("Integer\n");
    variant = QVariant(asInteger(rvalue));
    break;
  case STRSXP:
    // Rprintf("String\n");
    variant = QVariant(sexp2qstring(rvalue));
    break;
  case EXTPTRSXP:
    // Rprintf("External pointer\n");
    variant = qVariantFromValue(unwrapPointer(rvalue, void));
    break;
  default:
    error("Converting to QVariant: unhandled R type");
  }
  return variant;
}

/* create a QVariant of specific type by first creating a QVariant
   directly, and then attempting to coerce it to the required type.
*/
QVariant asQVariantOfType(SEXP rvalue, QMetaType::Type type) {
  QVariant direct = asQVariant(rvalue);
  QVariant ans;
  if (direct.canConvert((QVariant::Type)type)) { // handles a few cases
    direct.convert((QVariant::Type)type);
    return direct;
  }
  switch(type) {
    // FIXME: should verify that these pointers are of right R class
  case QMetaType::QObjectStar:
    ans = qVariantFromValue(unwrapQObject(rvalue, QObject));
    break;
  case QMetaType::QWidgetStar:
    ans = qVariantFromValue(unwrapQObject(rvalue, QWidget));
    break;
  case QMetaType::QCursor:
    break;
  case QMetaType::QDate:
    break;
  case QMetaType::QSize:
  case QMetaType::QSizeF:
    ans = QVariant(asQSizeF(rvalue));
    break;
  case QMetaType::QTime:
    break;
  case QMetaType::QVariantList:
    break;
  case QMetaType::QPolygon:
    break;
  case QMetaType::QColor:
    ans = QVariant(asQColor(rvalue));
    break;
  case QMetaType::QRectF:
  case QMetaType::QRect:
    ans = QVariant(asQRectF(rvalue));
    break;
  case QMetaType::QLine:
    break;
  case QMetaType::QTextLength:
    break;
  case QMetaType::QStringList:
    break;
  case QMetaType::QVariantMap:
    break;
  case QMetaType::QVariantHash:
    break;
  case QMetaType::QIcon:
    break;
  case QMetaType::QPen:
    break;
  case QMetaType::QLineF:
    break;
  case QMetaType::QTextFormat:
    break;
  case QMetaType::QPoint:
  case QMetaType::QPointF:
    ans = QVariant(asQPointF(rvalue));
    break;
  case QMetaType::QUrl:
    break;
  case QMetaType::QRegExp:
    break;
  case QMetaType::QDateTime:
    break;
  case QMetaType::QPalette:
    break;
  case QMetaType::QFont:
    ans = QVariant(asQFont(rvalue));
    break;
  case QMetaType::QBrush:
    break;
  case QMetaType::QRegion:
    break;
  case QMetaType::QBitArray:
    break;
  case QMetaType::QImage:
    break;
  case QMetaType::QKeySequence:
    break;
  case QMetaType::QSizePolicy:
    break;
  case QMetaType::QPixmap:
    break;
  case QMetaType::QLocale:
    break;
  case QMetaType::QBitmap:
    break;
  case QMetaType::QMatrix:
    ans = QVariant(asQMatrix(rvalue));
    break;
  case QMetaType::QTransform:
    break;
  case QMetaType::User:
    break;
  default:
    error("Converting to QVariant: unhandled Qt type");
  }
  if (!ans.isValid())
    error("Converting to QVariant: Qt type not yet implemented");
  return ans;
}

/* string handling */

const char ** asStringArray(SEXP s_strs) {
  const char **strs = (const char **)R_alloc(length(s_strs), sizeof(char *));
  for (int i = 0; i < length(s_strs); i++)
    strs[i] = CHAR(STRING_ELT(s_strs, i));
  return strs;
}
QString sexp2qstring(SEXP s) {
  if (!length(s))
    return QString();
  return QString::fromLocal8Bit(CHAR(asChar(s)));
}

SEXP asRStringArray(const char * const * strs) {
  SEXP ans;
  int n = 0;
  while(strs[n])
    n++;
  PROTECT(ans = allocVector(STRSXP, n));
  for (int i = 0; i < n; i++)
    SET_STRING_ELT(ans, i, mkChar(strs[i]));
  UNPROTECT(1);
  return ans;
}
SEXP qstring2sexp(QString s) {
  return ScalarString(mkChar(s.toLocal8Bit().data()));
}

/* geometry */
// FIXME: these and other asR functions need to set the class

SEXP asRRectF(QRectF rect) {
  SEXP rrect = allocMatrix(REALSXP, 2, 2);
  REAL(rrect)[0] = rect.left();
  REAL(rrect)[1] = rect.right();
  REAL(rrect)[2] = rect.top();
  REAL(rrect)[3] = rect.bottom();
  setAttrib(rrect, R_ClassSymbol, mkString("QRectF"));
  return rrect;
}
QRectF asQRectF(SEXP r) {
  double *rrect = REAL(r);
  return QRectF(QPointF(rrect[0], rrect[2]), QPointF(rrect[1], rrect[3]));
}

SEXP asRMatrix(QMatrix matrix, bool inverted) {
  bool ok = true;
  SEXP ans = R_NilValue;
  if (inverted)
    matrix = matrix.inverted(&ok);
  if (ok) {
    ans = allocMatrix(REALSXP, 3, 2);
    REAL(ans)[0] = matrix.m11();
    REAL(ans)[1] = matrix.m21();
    REAL(ans)[2] = matrix.dx();
    REAL(ans)[3] = matrix.m12();
    REAL(ans)[4] = matrix.m22();
    REAL(ans)[5] = matrix.dy();
  }
  setAttrib(ans, R_ClassSymbol, mkString("QMatrix"));
  return ans;
}
QMatrix asQMatrix(SEXP m) {
  double *rmatrix = REAL(m);
  return QMatrix(rmatrix[0], rmatrix[3], rmatrix[1], rmatrix[4],
                 rmatrix[2], rmatrix[5]);
}

SEXP asRPointF(QPointF point) {
  SEXP rpoint = allocVector(REALSXP, 2);
  REAL(rpoint)[0] = point.x(); REAL(rpoint)[1] = point.y();
  setAttrib(rpoint, R_ClassSymbol, mkString("QPointF"));
  return rpoint;
}
QPointF asQPointF(SEXP p) {
  double *rpoint = REAL(p);
  return QPointF(rpoint[0], rpoint[1]);
}

SEXP asRSizeF(QSizeF size) {
  SEXP rsize = allocVector(REALSXP, 2);
  REAL(rsize)[0] = size.width(); REAL(rsize)[1] = size.height();
  setAttrib(rsize, R_ClassSymbol, mkString("QSizeF"));
  return rsize;
}
QSizeF asQSizeF(SEXP s) {
  double *rsize = REAL(s);
  return QSizeF(rsize[0], rsize[1]);
}

/* Graphics */

QColor *asQColors(SEXP c) {
  int ncolors = length(c) / 4;
  QColor *colors = (QColor *)R_alloc(ncolors, sizeof(QColor));
  int *rcolors = INTEGER(c);
  for (int i = 0; i < ncolors; i++, rcolors += 4)
    colors[i] = QColor(rcolors[0], rcolors[1], rcolors[2], rcolors[3]);
  return colors;
}
QColor asQColor(SEXP c) {
  int *rcolor = INTEGER(c);
  return QColor(rcolor[0], rcolor[1], rcolor[2], rcolor[3]);
}
SEXP asRColor(QColor color) {
  SEXP rcolor = allocMatrix(INTSXP, 1, 4);
  int *rptr = INTEGER(rcolor);
  rptr[0] = color.red();
  rptr[1] = color.green();
  rptr[2] = color.blue();
  rptr[3] = color.alpha();
  return rcolor;
}

SEXP asRFont(QFont font) {
  static const char * fontNames[] =
    { "family", "pointsize", "weight", "italic", NULL };
  SEXP rfont = allocVector(VECSXP, 4);
  SET_VECTOR_ELT(rfont, 0, qstring2sexp(font.family()));
  SET_VECTOR_ELT(rfont, 1, ScalarInteger(font.pointSize()));
  SET_VECTOR_ELT(rfont, 2, ScalarInteger(font.weight()));
  SET_VECTOR_ELT(rfont, 3, ScalarLogical(font.italic()));
  setAttrib(rfont, R_NamesSymbol, asRStringArray(fontNames));
  return rfont;
}
QFont asQFont(SEXP f) {  
  return QFont(sexp2qstring(VECTOR_ELT(f, 0)), asInteger(VECTOR_ELT(f, 1)),
               asInteger(VECTOR_ELT(f, 2)), VECTOR_ELT(f, 3));
}
