#include <Rinternals.h>
#include <QString>

extern "C" {

  // R -> C/Qt
    const char ** asStringArray(SEXP s_strs);
    QString sexp2qstring(SEXP s);
  
  // C -> R
    SEXP asRStringArray(const char * const * strs);
    SEXP qstring2sexp(QString s);
}
