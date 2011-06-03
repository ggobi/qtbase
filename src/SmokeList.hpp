#ifndef SMOKE_LIST_H
#define SMOKE_LIST_H

#include <QList>
#include <smoke.h>

class SmokeList : public QList<Smoke *> {
public:
  SmokeList() { }
  SmokeList(const QList<Smoke *> &other) : QList<Smoke *>(other) { }

  Smoke::ModuleIndex findMethod(const char *c, const char *name);
};

#endif
