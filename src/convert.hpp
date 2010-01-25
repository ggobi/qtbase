#include <QVariant>
#include <QString>

// low-level conversion (reference wrapping)
#include "wrap.hpp" 

// high-level conversion (copying)

extern "C" {
  SEXP asRVariant(QVariant variant);
  QVariant asQVariant(SEXP rvalue);
  QVariant asQVariantOfType(SEXP rvalue, QMetaType::Type type);
  
  // R -> C/Qt
  const char ** asStringArray(SEXP s_strs);
  QString sexp2qstring(SEXP s);
  QByteArray asQByteArray(SEXP sexp);
  
  QRectF asQRectF(SEXP r);
  QPointF asQPointF(SEXP p);
  QSizeF asQSizeF(SEXP s);
  QMatrix asQMatrix(SEXP m);
  
  QColor *asQColors(SEXP c);
  QColor asQColor(SEXP c);
  QFont asQFont(SEXP f);
  
  // C -> R
  SEXP asRStringArray(const char * const * strs);
  SEXP qstring2sexp(QString s);
  SEXP asRByteArray(QByteArray s);
  
  SEXP asRRectF(QRectF rect);
  SEXP asRMatrix(QMatrix matrix, bool inverted);
  SEXP asRPointF(QPointF point);
  SEXP asRSizeF(QSizeF size);

  SEXP asRColor(QColor color);
  SEXP asRFont(QFont font);
}
