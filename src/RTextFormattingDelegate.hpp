/* Translates QVariant to text for Qt::DisplayRole using R's logic */

#include <QStyledItemDelegate>

class RTextFormattingDelegate : public QStyledItemDelegate {

public:
  RTextFormattingDelegate(QObject *parent = NULL) : QStyledItemDelegate(parent)
  {
  }
  QString displayText(const QVariant &value, const QLocale &locale) const;
  QWidget *createEditor(QWidget * parent, const QStyleOptionViewItem &,
                        const QModelIndex& index) const;
  void setModelData(QWidget *editor, QAbstractItemModel *model,
                    const QModelIndex &index ) const;
  void setEditorData(QWidget *editor, const QModelIndex &index) const;
};
