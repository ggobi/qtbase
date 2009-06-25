#include "RDynamicQObject.hpp"
#include "utils.hpp"

DynamicSlot *RDynamicQObject::createSlot(const char *slot) {
  Q_UNUSED(slot);
  return new RDynamicSlot(this);
}

SEXP asRValue(void *val, QByteArray type) {
  if (type == "int")
    return ScalarInteger(*(reinterpret_cast<int *>(val)));
  if (type == "const QString")
    return qstring2sexp(*(reinterpret_cast<const QString *>(val)));
  if (type == "bool")
    return ScalarLogical(*(reinterpret_cast<bool *>(val)));
  if (type == "QWidget*")
    return wrapQWidget(reinterpret_cast<QWidget *>(val));
  if (type == "QAction*" || type == "QAbstractButton*" ||
      type == "QWebFrame*" || type == "QNetworkReply*" || type == "QObject*")
    return wrapQObject(reinterpret_cast<QObject *>(val));
  if (type == "qint64")
    return ScalarReal(*(reinterpret_cast<qint64 *>(val)));
  if (type == "double")
    return ScalarReal(*(reinterpret_cast<double *>(val)));
  return R_NilValue; 
}

/* Types that occur more than once and have not been handled.
   Most of these require conversion to native R types (QRect, etc) or
   are non-QObjects and so need custom finalizers.
   
   Sorted in order from least to most occurrences:
  QSessionManager&                   Qt::Orientation 
  Qt::ToolButtonStyle                Qt::WindowStates 
  const QHttpResponseHeader&          const QItemSelection& 
  const QRectF&                     QDesignerFormWindowInterface* 
  QPrinter* 
  QSqlRecord&                      const QColor& 
  const QDate&                      const QNetworkProxy& 
  const QSize&            
                      const QFont& 
  const QList<QSslError>&

  These are pretty common (more common than qint64):
  QAuthenticator*             
  const QRect&                       QListWidgetItem* 
  QTableWidgetItem*                      QTreeWidgetItem* 
  const QUrl&
*/

/* NOTE: NO signals in Qt itself return a value! */
void setReturnValue(void *val, SEXP result, QByteArray type) {
  if (type == "bool") {
    bool *ptr = reinterpret_cast<bool *>(val);
    *ptr = asLogical(result);
  } else if (type == "int") {
    int *ptr = reinterpret_cast<int *>(val);
    *ptr = asInteger(result);
  } else if (type == "double") {
    double *ptr = reinterpret_cast<double *>(val);
    *ptr = asReal(result);
  } else if (type == "QString" || type == "const QString") {
    QString *ptr = reinterpret_cast<QString *>(val);
    *ptr = sexp2qstring(result);
  }
}

#define MIN(a, b) a < b ? a : b

void RDynamicSlot::call(QObject *sender, void **arguments) {
  Q_UNUSED(sender);
  SEXP user_data = object->userData(), function = object->function();
  bool have_user_data = user_data != NULL;
  QList<QByteArray> types = object->paramTypes();
  QByteArray returnType = object->returnType();
  SEXP lang, lang_tmp;
  int rnargs = length(FORMALS(function));
  int nargs = MIN(types.size(), rnargs - have_user_data);
  PROTECT(lang = allocVector(LANGSXP, nargs + 1 + have_user_data));
  SETCAR(lang, function);
  lang_tmp = CDR(lang);
  for (int i = 0; i < nargs; i++) {
    SETCAR(lang_tmp, asRValue(arguments[i+1], types[i]));
    lang_tmp = CDR(lang_tmp);
  }
  if (have_user_data)
    SETCAR(lang_tmp, user_data);
  SEXP ans = R_tryEval(lang, R_GlobalEnv, NULL);
  if (returnType.isEmpty())
    setReturnValue(arguments[0], ans, returnType);
}

/* For reference, here are the counts of the types used in signals: */

/*
  Feature                            QAbstractItemDelegate::EndEditHint 
  1                                  1 
  QAbstractSocket::SocketError       QAbstractSocket::SocketState 
  1                                  1 
  QClipboard::Mode                   QDBusPendingCallWatcher* 
  1                                  1 
  QDockWidget::DockWidgetFeatures    QHeaderView::ResizeMode 
  1                                  1 
  QImageReader::ImageReaderError     QLocalSocket::LocalSocketError 
  1                                  1 
  QLocalSocket::LocalSocketState     QMdiSubWindow* 
  1                                  1 
  QMovie::MovieState                 QNetworkReply::NetworkError 
  1                                  1 
  QProcess::ExitStatus               QProcess::ProcessError 
  1                                  1 
  QProcess::ProcessState             QSslSocket::SslMode 
  1                                  1 
  QStandardItem*                     QString 
  1                                  1 
  QSystemTrayIcon::ActivationReason  QTimeLine::State 
  1                                  1 
  QUndoStack*                        QWebHistoryItem* 
  1                                  1 
  QX11EmbedContainer::Error          QX11EmbedWidget::Error 
  1                                  1 
  Qt::DockWidgetArea                 Qt::DockWidgetAreas 
  1                                  1 
  Qt::DropAction                     Qt::SortOrder 
  1                                  1 
  Qt::ToolBarAreas                   const QDBusConnection& 
  1                                  1 
  const QDBusError&                  const QDBusMessage& 
  1                                  1 
  const QDateTime&                   const QList<QRectF>& 
  1                                  1 
  const QMap<QStringQUrl>&           const QModelIndexList& 
  1                                  1 
  const QNetworkRequest&             const QPoint& 
  1                                  1 
  const QScriptValue&                const QSizeF& 
  1                                  1 
  const QSslError&                   const QStringList& 
  1                                  1 
  const QTextBlock&                  const QTextCharFormat& 
  1                                  1 
  const QTextCursor&                 const QTime& 
  1                                  1 
  const QUrlInfo&                    const QVariant& 
  1                                  1 
  qreal                            quint16 
  1                                  1 
  QSessionManager&                   Qt::Orientation 
  2                                  2 
  Qt::ToolButtonStyle                Qt::WindowStates 
  2                                  2 
  const QHttpResponseHeader&          const QItemSelection& 
  2                                  2 
  const QRectF&                     QDesignerFormWindowInterface* 
  2                                  3 
  QObject*                          QPrinter* 
  3                                  3 
  QSqlRecord&                      const QColor& 
  3                                  3 
  const QDate&                      const QNetworkProxy& 
  3                                  3 
  const QSize&                       double 
  3                                  3 
  QNetworkReply*                    const QFont& 
  4                                  4 
  const QList<QSslError>&            QAbstractButton* 
  4                                  5 
  QAuthenticator*                    QWebFrame* 
  5                                  5 
  const QRect&                       QListWidgetItem* 
  5                                  8 
  QTableWidgetItem*                  qint64 
  8                                  8 
  QAction*                          QTreeWidgetItem* 
  9                                 10 
  const QUrl&                        QWidget* 
  10                                 14 
  const QModelIndex&                 bool 
  27                                 36 
  const QString&                     int 
  60                                142 
 */
  
