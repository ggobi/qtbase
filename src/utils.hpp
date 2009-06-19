
#ifndef WIN32
#include <unistd.h>
#endif

#include <R.h>
#include <Rinternals.h>
#include <R_ext/Error.h>


extern "C" {

    const char ** asStringArray(SEXP s_strs);

}
