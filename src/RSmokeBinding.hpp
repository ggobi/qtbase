#include <smoke.h>
#include <smoke/qt_smoke.h>

/*
 * This class will intercept all virtual method calls and will get
 * notified when an instance created by smoke gets destroyed.
 */

// FIXME: this should be constructed by the Class, with a special
// Moc subclass that handles the metacall stuff. In the current design
// a singleton instance is provided by the RQtModule. Each Class class
// can statically store its own singleton.

class SmokeObject;

typedef struct SEXPREC* SEXP;

class RSmokeBinding : public SmokeBinding
{
public:
  RSmokeBinding(Smoke *s) : SmokeBinding(s) {}
 
  void deleted(Smoke::Index classId, void *obj); 
  bool callMethod(Smoke::Index method, void *obj,
                  Smoke::Stack args, bool isAbstract); 
  char *className(Smoke::Index classId);

  Smoke *getSmoke() { return smoke; }

private:
  int qt_metacall(SmokeObject *so, QMetaObject::Call _c, int id, void **_o);

};
