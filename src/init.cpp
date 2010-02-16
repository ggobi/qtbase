#include "convert.hpp"

#include <Rinternals.h>
#include <R_ext/Rdynload.h>

void init_smoke();
void init_type_handlers();

extern "C" {
  
  // style.cpp
  SEXP qt_qsetStyleSheet(SEXP x, SEXP s);
  SEXP qt_qstyleSheet(SEXP x);

  // EventLoop.cpp 
  SEXP addQtEventHandler();
  SEXP cleanupQtApp();
  
  // this file
  void R_init_qtbase(DllInfo *dll);
    
  // signals
  SEXP qt_qconnect(SEXP x, SEXP user_data, SEXP handler, SEXP which);

  // metadata
  SEXP qt_qmethods(SEXP klass);
  SEXP qt_qenums(SEXP klass);
  SEXP qt_qproperties(SEXP x);
  SEXP qt_qclasses(SEXP rsmoke);

  // Smoke-specific metadata
  SEXP qt_qsmokes(void);

  // Moc-specific metadata
  SEXP qt_qnormalizedSignature(SEXP x);
  SEXP qt_qmocMethods(SEXP x);
  
  // dynamic invocation
  SEXP qt_qinvoke(SEXP method, SEXP self, SEXP args);
  SEXP qt_qinvokeStatic(SEXP method, SEXP smoke, SEXP klass, SEXP args);
  
  // user classes
  SEXP qt_qcast(SEXP x, SEXP className);
  SEXP qt_qenclose(SEXP x, SEXP fun);

  // registration of Smoke modules from other packages
  void registerRQtModule(Smoke *smoke);
}

#define CALLDEF(name, n)  {#name, (DL_FUNC) &name, n}

#define CALLDEF_COERCE(name) CALLDEF(qt_coerce_##name, 1)

static R_CallMethodDef CallEntries[] = {

    // special support for style sheets
    CALLDEF(qt_qsetStyleSheet, 2),
    CALLDEF(qt_qstyleSheet, 1),

    // event loop
    CALLDEF(addQtEventHandler, 0),
    CALLDEF(cleanupQtApp, 0),

    // signals
    CALLDEF(qt_qconnect, 5),

    // Moc metadata
    CALLDEF(qt_qmocMethods, 1),
    CALLDEF(qt_qnormalizedSignature, 1),

    // General metadata
    CALLDEF(qt_qmethods, 1),
    CALLDEF(qt_qenums, 1),
    CALLDEF(qt_qproperties, 1),
    CALLDEF(qt_qclasses, 1),

    // Smoke metadata
    CALLDEF(qt_qsmokes, 0),

    // Dynamic invocation
    CALLDEF(qt_qinvoke, 4),
    CALLDEF(qt_qinvokeStatic, 3),
    
    // User classes
    CALLDEF(qt_qcast, 2),
    CALLDEF(qt_qenclose, 2),

    // Explicit coercions
    CALLDEF_COERCE(QByteArray),
    CALLDEF_COERCE(QRectF),
    CALLDEF_COERCE(QRect),
    CALLDEF_COERCE(QTransform),
    CALLDEF_COERCE(QPointF),
    CALLDEF_COERCE(QPoint),
    CALLDEF_COERCE(QSizeF),
    CALLDEF_COERCE(QSize),
    CALLDEF_COERCE(QColor),
 
    {NULL, NULL, 0}
};


#define REG_CALLABLE(fun) R_RegisterCCallable("qtbase", #fun, (DL_FUNC) fun)

void R_init_qtbase(DllInfo *dll)
{
    init_smoke();
    init_type_handlers();
    
    // Register C routines
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);

    // Register for calling by other packages.
    REG_CALLABLE(_unwrapSmoke);
    REG_CALLABLE(wrapPointer);
    REG_CALLABLE(wrapSmoke);

    REG_CALLABLE(sexp2qstring);
    REG_CALLABLE(qstring2sexp);

    REG_CALLABLE(registerRQtModule);
}
