#include <QComboBox>
#include <QLineEdit>
#include <QDoubleValidator>
#include <QIntValidator>

#include <R_ext/PrtUtil.h>

#include "RTextFormattingDelegate.hpp"
#include "NameOnlyClass.hpp"
#include "SmokeObject.hpp"
#include "DataFrameModel.hpp"
#include "convert.hpp"

QString RTextFormattingDelegate::displayText (const QVariant &value,
                                              const QLocale &locale) const
{
  int w, d, e;
  QString str;
  
  switch((QMetaType::Type)value.type()) {
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

QWidget *
RTextFormattingDelegate::createEditor(QWidget * parent,
                                      const QStyleOptionViewItem &option,
                                      const QModelIndex& index) const
{
  const DataFrameModel *df_model =
    qobject_cast<const DataFrameModel *>(index.model());
  QWidget *editor = NULL;
  
  if (df_model) {
    SEXP sexp = df_model->dataFrameColumn(index, Qt::EditRole);
    if (isFactor(sexp)) { // Qt has no notion of factors
      QComboBox *combo = new QComboBox(parent);
      combo->addItems(from_sexp<QStringList>(getAttrib(sexp, R_LevelsSymbol)));
      combo->addItem("<NA>");
      editor = combo;
    } else if (isLogical(sexp)) { // Need to add 'NA' option
      QComboBox *combo = new QComboBox(parent);
      combo->addItem("FALSE");
      combo->addItem("TRUE");
      combo->addItem("NA");
      editor = combo;
    } else if (isNumber(sexp) && !isLogical(sexp)) {
      // line edit better than spin button
      editor = new QLineEdit(parent);
    }
  }

  if (!editor)
    editor = QStyledItemDelegate::createEditor(parent, option, index);

  return editor;
}

void RTextFormattingDelegate::setModelData(QWidget *editor,
                                           QAbstractItemModel *model,
                                           const QModelIndex &index ) const
{
  DataFrameModel *df_model = qobject_cast<DataFrameModel *>(model);
  bool set = false;
  if (df_model) {
    SEXP sexp = df_model->dataFrameColumn(index, Qt::EditRole);
    if (isLogical(sexp)) {
      QComboBox *combo = qobject_cast<QComboBox *>(editor);
      df_model->setData(index, combo->currentIndex() == 2 ? NA_LOGICAL :
                        combo->currentIndex(), Qt::EditRole);
      set = true;
    } else if (isFactor(sexp)) {
      QComboBox *combo = qobject_cast<QComboBox *>(editor);
      df_model->setData(index, combo->currentIndex() == combo->count() - 1 ?
                        NA_LOGICAL : combo->currentIndex() + 1);
      set = true;
    }
  }
  if (!set)
    QStyledItemDelegate::setModelData(editor, model, index);
}

void RTextFormattingDelegate::setEditorData(QWidget *editor,
                                            const QModelIndex &index) const
{
  const DataFrameModel *df_model =
    qobject_cast<const DataFrameModel *>(index.model());
  bool set = false;
  if (df_model) {
    SEXP sexp = df_model->dataFrameColumn(index, Qt::EditRole);
    switch(TYPEOF(sexp)) {
    case LGLSXP: {
      QComboBox *combo = qobject_cast<QComboBox *>(editor);
      int val = LOGICAL(sexp)[index.row()];
      combo->setCurrentIndex(val == NA_LOGICAL ? 2 : val);
      set = true;
      break;
    }
    case REALSXP: {
      QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor);
      double val = REAL(sexp)[index.row()];
      lineEdit->setText(ISNA(val) ? "NA" : R_IsNaN(val) ? "NaN" :
                        QString::number(val, 'f', DBL_DIG));
      set = true;
      break;
    }
    case INTSXP: {
      int val = INTEGER(sexp)[index.row()];
      if (!isFactor(sexp)) {
        QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor);
        lineEdit->setText(val == NA_INTEGER ? "NA" : QString::number(val));
      } else {
        QComboBox *combo = qobject_cast<QComboBox *>(editor);
        combo->setCurrentIndex(val == NA_INTEGER ?
                               combo->count() - 1 : val - 1);
      }
      set = true;
      break;
    }
    default:
      break;
    }
  }
  if (!set)
    QStyledItemDelegate::setEditorData(editor, index);
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
