#include <smoke.h>
#include <smoke/qt_smoke.h>

/*
 * This class will intercept all virtual method calls and will get
 * notified when an instance created by smoke gets destroyed.
 */
class RSmokeBinding : public SmokeBinding
{
public:
  RSmokeBinding(Smoke *s) : SmokeBinding(s) {}
 
  void deleted(Smoke::Index classId, void *obj); 
  bool callMethod(Smoke::Index method, void *obj,
                  Smoke::Stack args, bool isAbstract); 
  char *className(Smoke::Index classId);

  Smoke *getSmoke() { return smoke; }
};
