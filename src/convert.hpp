#include <QVariant>
#include <QString>

// low-level conversion (reference wrapping)
#include "wrap.hpp" 

// high-level conversion (copying)

SEXP asRVariant(QVariant variant);
QVariant asQVariant(SEXP rvalue);
QVariant asQVariantOfType(SEXP rvalue, QMetaType::Type type);

// R -> C/Qt
const char ** asStringArray(SEXP s_strs);
QString sexp2qstring(SEXP s);
  
// C -> R
SEXP asRStringArray(const char * const * strs);
SEXP qstring2sexp(QString s);
