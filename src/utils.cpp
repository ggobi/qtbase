
#include <R.h>
#include <Rinternals.h>
#include <R_ext/Parse.h>

extern "C" {

extern SEXP R_ParseVector(SEXP, int, ParseStatus *, SEXP);
SEXP evaluateInR(const char *cmd);
SEXP executeInR(const char *cmd);
static SEXP cmdExecRho;
static SEXP rcmdSym;
void init_utils();


} // end extern "C"


SEXP
evaluateInR(const char *cmd)
{
    SEXP cmdSexp, cmdExpr, ans = R_NilValue;
    ParseStatus status;
    PROTECT(cmdSexp = mkString(cmd));
    PROTECT(cmdExpr = R_ParseVector(cmdSexp, -1, &status, R_NilValue));
    if (status == PARSE_OK) {
        int i;
        for(i = 0; i < length(cmdExpr); i++) 
	    ans = eval(VECTOR_ELT(cmdExpr, i), R_GlobalEnv);
	UNPROTECT(2);
	return ans;
    }
    else {
	Rprintf("status not ok: %s\n", cmd);
	UNPROTECT(2);
	return R_NilValue;
    }
}



SEXP
executeInR(const char *cmd)
{
    SEXP cmdSexp;
    PROTECT(cmdSexp = mkString(cmd));
    defineVar(rcmdSym, cmdSexp, cmdExecRho);
    evaluateInR(".u_tryEval()");
    UNPROTECT(1);
    return R_NilValue;
}

void init_utils()
{
    rcmdSym = install("command");
    SEXP rho = R_FindNamespace(mkString("qtbase"));
    cmdExecRho = eval(install(".uEnv"), rho);
}

