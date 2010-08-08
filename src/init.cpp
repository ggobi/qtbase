#include "convert.hpp"

#include <Rinternals.h>
#include <R_ext/Rdynload.h>

void init_smoke();
void init_type_handlers();

extern "C" {
  
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
  SEXP qt_qmetacall(SEXP x, SEXP s_call, SEXP s_id, SEXP s_args);
  SEXP qt_qnewMetaObject(SEXP x, SEXP rstringdata, SEXP rdata);
  SEXP qt_qmetaInvoke(SEXP x, SEXP s_id, SEXP s_args);
  
  // dynamic invocation
  SEXP qt_qinvoke(SEXP method, SEXP self, SEXP args);
  SEXP qt_qinvokeStatic(SEXP method, SEXP smoke, SEXP klass, SEXP args);
  
  // user classes
  SEXP qt_qcast(SEXP x, SEXP className);
  SEXP qt_qenclose(SEXP x, SEXP fun);

  // Invoke a Smoke method with R types
  SEXP invokeSmokeMethod(Smoke::ModuleIndex m, SEXP x, SEXP args);

  // registration of Smoke modules from other packages
  Smoke *registerRQtModule(Smoke *smoke);

  // DataFrameModel
  SEXP qt_qdataFrameModel();
  SEXP qt_qsetDataFrame(SEXP rmodel, SEXP df, SEXP roles, SEXP rowHeader,
                        SEXP colHeader);
  SEXP qt_qdataFrame(SEXP rmodel);
}

#define CALLDEF(name, n)  {#name, (DL_FUNC) &name, n}

#define CALLDEF_COERCE(name) CALLDEF(qt_coerce_##name, 1)

static R_CallMethodDef CallEntries[] = {

    // event loop
    CALLDEF(addQtEventHandler, 0),
    CALLDEF(cleanupQtApp, 0),

    // signals
    CALLDEF(qt_qconnect, 5),

    // Moc metadata
    CALLDEF(qt_qmocMethods, 1),
    CALLDEF(qt_qnormalizedSignature, 1),
    CALLDEF(qt_qmetacall, 4),
    CALLDEF(qt_qnewMetaObject, 3),
    CALLDEF(qt_qmetaInvoke, 3),

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
    CALLDEF_COERCE(QRectF),
    CALLDEF_COERCE(QRect),
    CALLDEF_COERCE(QTransform),
    CALLDEF_COERCE(QPointF),
    CALLDEF_COERCE(QPoint),
    CALLDEF_COERCE(QPolygonF),
    CALLDEF_COERCE(QPolygon),
    CALLDEF_COERCE(QSizeF),
    CALLDEF_COERCE(QSize),
    CALLDEF_COERCE(QColor),

    // DataFrame
    CALLDEF(qt_qdataFrameModel, 0),
    CALLDEF(qt_qdataFrame, 1),
    CALLDEF(qt_qsetDataFrame, 5),
    
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
    REG_CALLABLE(_wrapSmoke);

    REG_CALLABLE(invokeSmokeMethod);
    
    REG_CALLABLE(sexp2qstring);
    REG_CALLABLE(qstring2sexp);

    REG_CALLABLE(registerRQtModule);
}
