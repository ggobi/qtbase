#include <R_ext/PrtUtil.h>

#include "RTextFormattingDelegate.hpp"
#include "NameOnlyClass.hpp"
#include "SmokeObject.hpp"

QString RTextFormattingDelegate::displayText (const QVariant &value,
                                              const QLocale &locale) const
{
  int w, d, e;
  QString str;
  
  switch(value.type()) {
  case QMetaType::Bool:
    {
      int b = value.value<int>();
      formatLogical(&b, 1, &w);
      str = QString(EncodeLogical(b, w));
    }
    break;
  case QMetaType::Int:
  case QMetaType::UInt:
  case QMetaType::Long:
  case QMetaType::Short:
  case QMetaType::UShort:
    {
      int n = value.value<int>();
      formatInteger(&n, 1, &w);
      str = QString(EncodeInteger(n, w));
    }
    break;
  case QMetaType::Double:
  case QMetaType::LongLong:
  case QMetaType::ULong:
  case QMetaType::ULongLong:
  case QMetaType::Float:
    {
      double n = value.value<double>();
      formatReal(&n, 1, &w, &d, &e, 0);
      str = QString(EncodeReal(n, w, d, e, '.'));
    }
    break;
  default:
    str = QStyledItemDelegate::displayText(value, locale);
  }

  return str;
}

#include "wrap.hpp"

extern "C"
SEXP qt_qrTextFormattingDelegate(SEXP rparent) {
  static Class *delegateClass =
    new NameOnlyClass("RTextFormattingDelegate",
                      Class::fromName("QStyledItemDelegate"));
  QObject *parent = unwrapSmoke(rparent, QObject);
  SmokeObject *so =
    SmokeObject::fromPtr(new RTextFormattingDelegate(parent),
                         Class::fromName("QStyledItemDelegate"), true);
  so->cast(delegateClass);
  return so->sexp();
}
