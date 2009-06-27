#include <QTimer>
#include "wrappers.h"

extern "C" SEXP qt_qtimer(void)
{
  return wrapQObject(new QTimer());
}
