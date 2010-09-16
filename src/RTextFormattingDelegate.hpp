/* Translates QVariant to text for Qt::DisplayRole using R's logic */

#include <QStyledItemDelegate>

class RTextFormattingDelegate : public QStyledItemDelegate {

public:
  QString displayText(const QVariant &value, const QLocale &locale) const;
  
};
