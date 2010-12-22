#include <QByteArray>
#include "MocStack.hpp"
#include "SmokeStack.hpp"
#include "SmokeType.hpp"
#include "SmokeClass.hpp"

MocStack::MocStack(void **o, int size) : _size(size), _o(o), _allocated(false) {
}
MocStack::MocStack(const SmokeStack &smoke, QVector<SmokeType> types)
  : _size(smoke.size()), _o(new void*[_size]), _allocated(true)
{
  setFromSmoke(smoke.items(), types);
}

SmokeStack MocStack::toSmoke(QVector<SmokeType> types) {
  SmokeStack stack(types.size());
  setSmoke(stack.items(), types);
  return stack;
}

void MocStack::returnToSmoke(const SmokeStack &stack, const SmokeType &type) {
  setSmokeItem(stack.items(), _o[0], type);
}
  
void
MocStack::returnFromSmoke(const SmokeStack &stack, const SmokeType &type) {
  Smoke::StackItem item = stack.ret();
  void *val = _o[0];
  if (type.isVoid())
    return;
  switch(type.elem()) {
  case Smoke::t_bool:
    *reinterpret_cast<bool *>(val) = item.s_bool;
    break;
  case Smoke::t_char:
    *reinterpret_cast<char *>(val) = item.s_char;
    break;
  case Smoke::t_uchar:
    *reinterpret_cast<unsigned char *>(val) = item.s_uchar;
    break;
  case Smoke::t_short:
    *reinterpret_cast<short *>(val) = item.s_short;
    break;
  case Smoke::t_ushort:
    *reinterpret_cast<unsigned short *>(val) = item.s_ushort;
    break;
  case Smoke::t_int:
    *reinterpret_cast<int *>(val) = item.s_int;
    break;
  case Smoke::t_uint:
    *reinterpret_cast<unsigned int *>(val) = item.s_uint;
    break;
  case Smoke::t_long:
    *reinterpret_cast<long *>(val) = item.s_long;
    break;
  case Smoke::t_ulong:
    *reinterpret_cast<unsigned long *>(val) = item.s_ulong;
    break;
  case Smoke::t_float:
    *reinterpret_cast<float *>(val) = item.s_float;
    break;
  case Smoke::t_double:
    *reinterpret_cast<double *>(val) = item.s_double;
    break;
  case Smoke::t_enum:
    *reinterpret_cast<int *>(val) = item.s_enum;
    break;
  case Smoke::t_class:
  case Smoke::t_voidp:
    if (strchr(type.name(), '*') != 0)
      *reinterpret_cast<void **>(val) = item.s_voidp;
    else {
      // It looks like we can just set our own pointer
      _o[0] = item.s_class;
      // Qt will just copy it if it is different from what it
      // allocated and is smart enough to free the original memory.
    }
    break;
  }
}

void MocStack::setFromSmoke(Smoke::Stack stack, QVector<SmokeType> types)
{
  if (!types[0].fitsStack()) {
    // allocate memory for return value; marshalling code should free
    stack[0].s_voidp =
      QMetaType::construct(QMetaType::type(types[0].className()));
  }
  for (int i = 0; i < _size; i++) {
    Smoke::StackItem *si = stack + i;
    SmokeType t = types[i];
    void *p = NULL;
    if (t.isVoid())
      continue;
    switch(t.elem()) {
    case Smoke::t_bool:
      p = &si->s_bool;
      break;
    case Smoke::t_char:
      p = &si->s_char;
      break;
    case Smoke::t_uchar:
      p = &si->s_uchar;
      break;
    case Smoke::t_short:
      p = &si->s_short;
      break;
    case Smoke::t_ushort:
      p = &si->s_ushort;
      break;
    case Smoke::t_int:
      p = &si->s_int;
      break;
    case Smoke::t_uint:
      p = &si->s_uint;
      break;
    case Smoke::t_long:
      p = &si->s_long;
      break;
    case Smoke::t_ulong:
      p = &si->s_ulong;
      break;
    case Smoke::t_float:
      p = &si->s_float;
      break;
    case Smoke::t_double:
      p = &si->s_double;
      break;
    case Smoke::t_enum:
      {
        // allocate a new enum value
        Smoke::EnumFn fn = SmokeClass(t).enumFn();
        if (!fn) {
          qWarning("Unknown enumeration %s", t.name());
          p = new int((int)si->s_enum);
          break;
        }
        Smoke::Index id = t.typeId();
        (*fn)(Smoke::EnumNew, id, p, si->s_enum);
        (*fn)(Smoke::EnumFromLong, id, p, si->s_enum);
        // FIXME: MEMORY LEAK
        break;
      }
    case Smoke::t_class:
    case Smoke::t_voidp:
      if (strchr(t.name(), '*') != 0) {
        p = &si->s_voidp;
      } else {
        p = si->s_voidp;
      }
      break;
    default:
      p = 0;
      break;
    }
    _o[i] = p;
  }
}

void MocStack::setSmokeItem(Smoke::StackItem *item, void *o,
                            const SmokeType &type)
{
  void *p = o;
  if (type.isVoid())
    return;
  switch(type.elem()) {
  case Smoke::t_bool:
    item[0].s_bool = *(bool*)o;
    break;
  case Smoke::t_char:
    item[0].s_char = *(char*)o;
    break;
  case Smoke::t_uchar:
    item[0].s_uchar = *(unsigned char*)o;
    break;
  case Smoke::t_short:
    item[0].s_short = *(short*)p;
    break;
  case Smoke::t_ushort:
    item[0].s_ushort = *(unsigned short*)p;
    break;
  case Smoke::t_int:
    item[0].s_int = *(int*)p;
    break;
  case Smoke::t_uint:
    item[0].s_uint = *(unsigned int*)p;
    break;
  case Smoke::t_long:
    item[0].s_long = *(long*)p;
    break;
  case Smoke::t_ulong:
    item[0].s_ulong = *(unsigned long*)p;
    break;
  case Smoke::t_float:
    item[0].s_float = *(float*)p;
    break;
  case Smoke::t_double:
    item[0].s_double = *(double*)p;
    break;
  case Smoke::t_enum:
    {
      Smoke::EnumFn fn = SmokeClass(type).enumFn();
      if (!fn) {
        qWarning("Unknown enumeration %s", type.name());
        item[0].s_enum = *(int*)p;
        break;
      }
      Smoke::Index id = type.typeId();
      (*fn)(Smoke::EnumToLong, id, p, item[0].s_enum);
    }
    break;
  case Smoke::t_class:
  case Smoke::t_voidp:
    if (strchr(type.name(), '*') != 0) {
      item[0].s_voidp = *(void **)p;
    } else {
      item[0].s_voidp = p;
    }
    break;
  }
}
void MocStack::setSmoke(Smoke::Stack stack, QVector<SmokeType> types)
{
  for (int i = 0; i < _size; i++) {
    SmokeType t = types[i];
    if (!t.isVoid())
      setSmokeItem(stack + i, _o[i], t);
  }
}

MocStack::~MocStack() {
  if (_allocated)
    delete _o;
}
