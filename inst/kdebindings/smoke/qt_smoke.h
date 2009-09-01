#ifndef QT_SMOKE_H
#define QT_SMOKE_H

#include <smoke.h>

// Defined in smokedata.cpp, initialized by init_qt_Smoke(), used by all .cpp files
extern SMOKE_EXPORT Smoke* qt_Smoke;
extern SMOKE_EXPORT void init_qt_Smoke();

#ifndef QGLOBALSPACE_CLASS
#define QGLOBALSPACE_CLASS
class QGlobalSpace { };
#endif

#endif
