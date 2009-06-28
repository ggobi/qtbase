#include <QTimer>
#include "wrap.hpp"

extern "C" SEXP qt_qtimer(void)
{
  return wrapQObject(new QTimer());
}
