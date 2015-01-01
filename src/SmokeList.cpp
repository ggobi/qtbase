#include <QByteArray>
#include <qt_smoke.h>

#include "SmokeList.hpp"


Smoke::ModuleIndex
SmokeList::findMethod(const char *className, const char *name) {
  Smoke::ModuleIndex meth = qt_Smoke->findMethod(className, name);
  if (!meth.index && !qstrcmp(className, "QGlobalSpace")) {
    // since every smoke module defines a class 'QGlobalSpace' we
    // can't rely on the classMap, so we search for methods by hand
    Smoke *s;
    foreach (s, *this) {
      meth = s->findMethod("QGlobalSpace", name);
      if (meth.index) break;
    }
  }
  return meth;
}
