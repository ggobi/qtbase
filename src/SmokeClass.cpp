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
  static QHash<QByteArray,Smoke::ModuleIndex*> cache;
  SmokeMethodCache() { }
};

// store index as pointer to detect cache misses
QHash<QByteArray,Smoke::ModuleIndex*> SmokeMethodCache::cache;

Smoke::ModuleIndex SmokeMethodCache::find(const MethodCall &call) {
  static Smoke::ModuleIndex missing(NULL, -1);
  if (call.args()) { // FIXME: caching does not work yet for foreign calls
    Smoke::ModuleIndex *index = cache[call.cacheKey()];
    if (index)
      return *index;
  }
  return missing;
}

void SmokeMethodCache::insert(const MethodCall &call,
                              const Smoke::ModuleIndex &index)
{
  if (call.args()) {
    cache[call.cacheKey()] = new Smoke::ModuleIndex(index);
  }
}

QByteArray SmokeClass::mungedMethodName(const MethodCall &call) const {
  SEXP args = call.args();
  QByteArray munged = QByteArray(call.method()->name());
  if (!args) // does not yet work for foreign calls
    return munged;
  for (int i = 0; i < length(args); i++) {
    SEXP arg = VECTOR_ELT(args, i);
    if (TYPEOF(arg) == RAWSXP || (isVectorAtomic(arg) && length(arg) == 1))
      munged += "$";
    else if (isNull(arg) || isEnvironment(arg))
      munged += "#";
    else munged += "?";
  }
  return munged;   
}

Smoke::ModuleIndex SmokeClass::findIndex(const MethodCall& call) const
{
  Smoke::ModuleIndex meth = SmokeMethodCache::find(call);
  if (meth.index != -1) // cache hit, return immediately
    return meth;
  Method *m = call.method();
  QByteArray munged = mungedMethodName(call);
  meth = _smoke->findMethod(name(), munged);
  if (meth.index) {
    Smoke::Index i = _smoke->methodMaps[meth.index].method;
    if (i > 0)
      meth.index = i;
    else if (i < 0) { // uh oh, multiple matches
      int bestMatch = -1;
      Smoke::Index ambig, bestMethod;
      SEXP rargs = call.args();
      bool ambiguous = true;
      i = -i;
      // We do not (yet?) support ambiguous Smoke calls
      if (!rargs) { 
        meth.index = -1;
        return meth;
      }
      while ((ambig = _smoke->ambiguousMethodList[i])) {
        int curMatch = 0;
        Smoke::Index *args = _smoke->argumentList + _smoke->methods[ambig].args;
        // score each argument
        for (int j = 0; args[j]; j++) {
          curMatch +=
            MethodCall::scoreArg(VECTOR_ELT(rargs, j), _smoke, args[j]);
          //qDebug("curMatch: %d", curMatch);
        }
        ambiguous = curMatch == bestMatch; 
        if (curMatch > bestMatch) {
          //qDebug("new best match: %d, old: %d", curMatch, bestMatch);
          bestMatch = curMatch;
          bestMethod = ambig;
        }
        i++;
      }
      if (ambiguous)
        error("Unable to disambiguate method %s::%s", name(), m->name());
      meth.index = bestMethod;
    } else error("Corrupt method %s::%s", name(), m->name());
    SmokeMethodCache::insert(call, meth); // cache
  }
  return meth;
}

Method *SmokeClass::findMethod(const MethodCall &call) const {
  Method *method = NULL;
  Smoke::ModuleIndex ind = findIndex(call);
  if (ind.index > 0)
    method = new SmokeMethod(ind);
  return method;
}

QList<const Class *> SmokeClass::parents() const {
  if (_parents.isEmpty()) {
    Smoke::Index *parents = _smoke->inheritanceList + _c->parents;
    for(int i = 0; parents[i]; i++) {
      const Class *c = Class::fromSmokeId(_smoke, parents[i]);
      _parents.append(c);
    }
  }
  return _parents;
}

void SmokeClass::findMethodRange() {
  Smoke::Index imax = _smoke->numMethodMaps;
  Smoke::Index imin = 0, icur = -1;
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
}

QList<Method *> SmokeClass::methods(Method::Qualifiers qualifiers) const {
  QList<Method *> meths;
  unsigned char flags = Smoke::mf_internal | Smoke::mf_enum;
  for (Smoke::Index i = methmin; i <= methmax; i++) {
    Smoke::Index ix = _smoke->methodMaps[i].method;
    if (ix >= 0) {	// single match
      if ((_smoke->methods[ix].flags & flags) == 0)
        if ((SmokeMethod(_smoke, ix).qualifiers() & qualifiers) == qualifiers)
          meths << new SmokeMethod(_smoke, ix);
    } else {		// multiple match
      ix = -ix;		// turn into ambiguousMethodList index
      Smoke::Index ambig;
      while ((ambig = _smoke->ambiguousMethodList[ix])) {
        Smoke::Method &methodRef = _smoke->methods[ambig];
        if ((methodRef.flags & flags) == 0) {
          SmokeMethod method(_smoke, ambig);
          if ((method.qualifiers() & qualifiers) == qualifiers)
            meths << new SmokeMethod(_smoke, ambig);
        }
        ix++;
      }
    }
  }
  foreach(const Class *p, parents())
    meths.append(p->methods(qualifiers | Method::NotPrivate));
  return meths;
}

bool
SmokeClass::hasMethod(const char *name, Method::Qualifiers qualifiers) const {
  /* Smoke requires a munged name, but we do not have one without a
     call, thus we precompute combined qualifiers of each name. This
     will break if the overloads differ by both access and staticness. */
  if (_methodQuals.isEmpty()) {
    QList<Method *> meths = methods();
    foreach(Method *m, meths) {
      _methodQuals[m->name()] = _methodQuals[m->name()] | m->qualifiers();
      delete m;
    }
  }
  Method::Qualifiers q = _methodQuals[name];
  return q && ((q & qualifiers) == qualifiers);
}

bool SmokeClass::implementsMethod(const char *name) const {
  Smoke::Index nameInd = _smoke->idMethodName(name).index;
  Smoke::Index methInd = _smoke->idMethod(_id, nameInd).index;
  return methInd > 0;
}

/* Enums */

QHash<const char *, int> SmokeClass::enumValues() const {
  if (!enumValuesCached) {
    _enumValues = createEnumValuesMap();
    enumValuesCached = true;
  }
  return _enumValues;
}

QHash<const char *, int> SmokeClass::createEnumValuesMap() const {
  QHash<const char *, int> values;
  Smoke::StackItem stack[1];
  for (int i = methmin; i <= methmax; i++) {
    Smoke::Method m = _smoke->methods[_smoke->methodMaps[i].method];
    if ((m.flags & Smoke::mf_ctor))
      continue; // constructors are capitalized, so can be mixed-in
    if ((m.flags & Smoke::mf_enum) == 0)
      break;
    (*_c->classFn)(m.method, 0, stack);
    values[_smoke->methodNames[m.name]] = stack[0].s_enum;
  }
  foreach(const Class *p, parents())
    values.unite(p->enumValues());
  return values;
}

Property *SmokeClass::property(const char *name) const {
  Q_UNUSED(name);
  return NULL;
}
