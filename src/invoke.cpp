/* experimental support for dynamic invocation of meta methods */
#include <QObject>
#include <QMetaObject>
#include <QMetaMethod>
#include <QGenericArgument>

#include "convert.hpp"

extern "C" SEXP qt_qinvoke(SEXP robj, SEXP rmethod, SEXP rargs) {
  QObject *obj = unwrapQObject(robj, QObject);
  const QMetaObject *meta = obj->metaObject();
  QMetaMethod method = meta->method(meta->indexOfMethod(CHAR(asChar(rmethod))));
  /* Very few slots have return values, might be worth it for bool and int
  QGenericReturnArgument retArg =
    genericReturnArgumentForType(meta.typeName());
  */
  QGenericArgument args[10];
  QList<QByteArray> paramTypes = method.parameterTypes();
  if (length(rargs) > paramTypes.size())
    error("Too many arguments for method");
  for (int i = 0; i < length(rargs); i++) {
    SEXP rarg = VECTOR_ELT(rargs, i);
    QByteArray type = paramTypes[i];
    QGenericArgument arg;
    if (type == "int") {
      arg = Q_ARG(int, asInteger(rarg));
    } else if (type == "bool") {
      arg = Q_ARG(bool, asLogical(rarg));
    } else if (type == "const QString") {
      arg = Q_ARG(QString, sexp2qstring(rarg));
    } else if (type == "QWidget*")
      arg = Q_ARG(QWidget*, unwrapQWidget(rarg));
    args[i] = arg;
    //args[i] = asQGenericArgument(VECTOR_ELT(rargs, i), paramTypes[i]);
  }
  bool success = method.invoke(obj, /*retArg,*/args[0], args[1], args[2],
                               args[3], args[4], args[5], args[6], args[7],
                               args[8], args[9]);
  if (!success)
    error("method invocation failed, check arguments");
  return robj; /*asRGenericReturnArgument(retArg);*/
}

/* Here are some counts of the types involved in slots:
   Feature 1 
   Priority  1 
   QAbstractItemView* 1 
   QAction* 1 
   QDBusReply<QDBusConnectionInterface::RegisterServiceReply> 1 
   QDBusReply<QString> 1 
   QDBusReply<QStringList> 1 
   QDBusReply<void> 1 
   QHelpEvent* 1 
   QMdiSubWindow* 1 
   QMovie* 1 
   QPrinter::Orientation 1 
   QToolButton::TextPosition 1 
   QUndoGroup* 1 
   QVariant 1 
   QXmlStreamReader* 1 
   Qt::Alignment 1 
   ServiceQueueOptions 1 
   ViewMode 1 
   ZoomMode 1 
   const QDateTime& 1 
   const QItemSelection& 1 
   const QList<QHelpSearchQuery>& 1 
   const QList<QRectF>& 1 
   const QListWidgetItem* 1 
   const QPicture& 1 
   const QPixmap& 1 
   const QPoint& 1 
   const QRect& 1 
   const QStyleOptionViewItem& 1 
   const QTableWidgetItem* 1 
   const QTime& 1 
   const QUrl& 1 
   const QVariant& 1 
   const bool 1 
   const int 1 
   QDBusReply<bool> 2 
   QDBusReply<uint> 2 
   QObject* 2 
   QPrinter*   2 
   QUndoStack* 2 
   Qt::ToolButtonStyle 2 
   const QByteArray& 2 
   const QFont& 2 
   const QSize& 2 
   QAbstractItemView::ScrollHint 3 
   QItemSelectionModel::SelectionFlags 3 
   QPainter* 3 
   Qt::Orientation 3 
   const QColor& 3 
   const QTreeWidgetItem* 3 
   double 3 
   const QDate& 4 
   QDesignerFormWindowInterface* 5 
   qreal 5 
   const QRectF& 6 
   QWidget* 10 
   const QModelIndex& 11 
   const QString& 50 
   bool 58 
   int 68
   void 410
*/
/* Return types specifically:
   QDBusReply<QDBusConnectionInterface::RegisterServiceReply> 1 
   QDBusReply<QString> 1 
   QDBusReply<QStringList> 1 
   QDBusReply<void> 1 
   QVariant 1 
   QDBusReply<bool> 2 
   QDBusReply<uint> 2 
   int 4 
   bool   12 
   void 410
*/
