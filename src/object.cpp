
#include <QObject>

#ifndef WIN32
#include <unistd.h>
#endif

#include <R.h>
#include <Rinternals.h>
#include <R_ext/Error.h>

// R-level interface to constructor

extern "C" {

SEXP new_QObject(QObject *x);
SEXP finalize_QObject(SEXP extp);

}


SEXP
new_QObject(QObject *x)
{
    SEXP ans, dtag = R_NilValue; 
    // SEXP dtag = PROTECT(duplicate(tag));
    // R_PreserveObject(extra_info);

    // set dynamic property indicating that an R reference exists to this pointer
    x->setProperty("R_has_reference", true);
    ans = PROTECT(R_MakeExternalPtr((void*) x, dtag, R_NilValue));
    UNPROTECT(1);
    return ans;
}

SEXP
finalize_QObject(SEXP extp)
{
    
    // (1) Set flag in object saying that R is no longer pointing to
    // it.  (2) Delete the widget it if it is a top-level widget, but
    // not otherwise.

    // Objects can be deleted here, but could also deleted by Qt
    // (typically when deleting its parent).  The idea is that Qt
    // should never delete a widget if it has an R reference pointing
    // to it; a delete event should eventually be called on it when
    // the finalizer runs.  (But how to ensure that?)  On the other
    // hand, child objects will not be deleted here, but will be
    // deleted by Qt when their parent is finalized and deleted

    QObject *p = (QObject *) R_ExternalPtrAddr(extp);
    if (!p) return R_NilValue;
    SEXP oldtag = PROTECT(R_ExternalPtrTag(extp));
    R_SetExternalPtrTag(extp, R_NilValue);
    p->setProperty("R_has_reference", false);
    R_ClearExternalPtr(extp);
    if (!p->parent()) { // top-level object, so delete
	// Rprintf("Top-level; Deleting. ");
	delete p;
    }
    UNPROTECT(1);
    return oldtag;
}


