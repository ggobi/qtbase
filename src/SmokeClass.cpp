#include <QHash>

#include "SmokeClass.hpp"
#include "SmokeMethod.hpp"
#include "MethodCall.hpp"

#include <Rinternals.h>

// FIXME: Can the caching be generalized? E.g. cache the Method*?
// Need to profile first

class SmokeMethodCache {
public:
  static Smoke::ModuleIndex find(const MethodCall &call);
  static void insert(const MethodCall &call,
                     const Smoke::ModuleIndex &method);
private:
  static QHash<QByteArray,Smoke::ModuleIndex*> *cache;
  SmokeMethodCache() { }
};

// store index as pointer to detect cache misses
QHash<QByteArray,Smoke::ModuleIndex*>* SmokeMethodCache::cache = NULL;

Smoke::ModuleIndex SmokeMethodCache::find(const MethodCall &call) {
  static Smoke::ModuleIndex missing = { NULL, -1 };
  if (call.args()) { // FIXME: caching does not work yet for foreign calls
    Smoke::ModuleIndex *index = (*cache)[call.cacheKey()];
    if (index)
      return *index;
  }
  return missing;
}

void SmokeMethodCache::insert(const MethodCall &call,
                              const Smoke::ModuleIndex &index)
{
  if (call.args()) {
    if (!cache)
      cache = new QHash<QByteArray,Smoke::ModuleIndex*>;
    (*cache)[call.cacheKey()] = new Smoke::ModuleIndex(index);
  }
}

Smoke::ModuleIndex SmokeClass::selectIndex(const MethodCall& call) const
{
  Smoke::ModuleIndex meth = SmokeMethodCache::find(call);
  if (meth.index != -1) // cache hit, return immediately
    return meth;
  Smoke::ModuleIndex ind = { _smoke, _id };
  Method *m = call.method();
  meth = _smoke->findMethod(ind, _smoke->idMethodName(m->name()));
  if (meth.index) {
    Smoke::Index i = _smoke->methodMaps[meth.index].method;
    if (i > 0)
      meth.index = i;
    else if (i < 0) { // uh oh, multiple matches
      int bestMatch = -1;
      Smoke::Index ambig, bestMethod;
      SEXP rargs = call.args();
      if (!rargs) { // We do not (yet?) support Smoke calls
        meth.index = -1;
        return meth;
      }
      bool ambiguous = true;
      i = -i;
      while ((ambig = _smoke->ambiguousMethodList[i])) {
        int curMatch = 0;
        Smoke::Index *args = _smoke->argumentList + _smoke->methods[ambig].args;
        for (int j = 0; args[j]; j++) { // score each argument
          curMatch +=
            MethodCall::scoreArg(VECTOR_ELT(rargs, j), _smoke, args[j]);
        }            
        if (curMatch > bestMatch) {
          bestMatch = curMatch;
          bestMethod = ambig;
        }
        ambiguous = curMatch == bestMatch; 
        i++;
      }
      if (ambiguous)
        error("Unable to disambiguate method %s::%s", name(), m->name());
      meth.index = bestMethod;
    } else error("Corrupt method %s::%s", name(), m->name());
    SmokeMethodCache::insert(call, meth); // remember for next time
  }
  return meth;
}

Method *SmokeClass::findMethod(const MethodCall &call) const {
  Method *method = NULL;
  Smoke::ModuleIndex ind = selectIndex(call);
  if (ind.index > 0)
    method = new SmokeMethod(ind);
  return method;
}

// Caller needs to free the elements
QList<const Class *> SmokeClass::parents() const {
  QList<const Class *> classes;
  Smoke::Index *parents = _smoke->inheritanceList + _c->parents;
  for(int i = 0; parents[i]; i++) {
    const Class *c = Class::fromSmokeId(_smoke, parents[i]);
    classes.append(c);
  }
  return classes;
}

QList<Method *> SmokeClass::methods() const {
  QList<Method *> meths;
  Smoke::Index imax = _smoke->numMethodMaps;
  Smoke::Index imin = 0, icur = -1, methmin = -1, methmax = -1;
  int icmp = -1;
  while(imax >= imin) {
    icur = (imin + imax) / 2;
    icmp = _smoke->leg(_smoke->methodMaps[icur].classId, _id);
    if (icmp == 0) {
      Smoke::Index pos = icur;
      while (icur && _smoke->methodMaps[icur-1].classId == _id)
        icur--;
      methmin = icur;
      icur = pos;
      while(icur < imax && _smoke->methodMaps[icur+1].classId == _id)
        icur ++;
      methmax = icur;
      break;
    }
    if (icmp > 0)
      imax = icur - 1;
    else
      imin = icur + 1;
  }
  if (icmp == 0) {
    for (Smoke::Index i = methmin; i <= methmax; i++) {
      Smoke::Index ix = _smoke->methodMaps[i].method;
      if (ix >= 0) {	// single match
        if ((_smoke->methods[ix].flags & Smoke::mf_internal) == 0)
          meths << new SmokeMethod(_smoke, ix);
      } else {		// multiple match
        ix = -ix;		// turn into ambiguousMethodList index
        while (_smoke->ambiguousMethodList[ix]) {
          Smoke::Method &methodRef =
            _smoke->methods[_smoke->ambiguousMethodList[ix]];
          if ((methodRef.flags & Smoke::mf_internal) == 0)
            meths << new SmokeMethod(_smoke, _smoke->ambiguousMethodList[ix]);
          ix++;
        }
      }
    }
  }
  return meths;
}
