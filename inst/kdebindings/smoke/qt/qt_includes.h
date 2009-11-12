#include <QtCore/QtCore>
#ifdef QT_QTGUI
#include <QtGui/QtGui>
#endif
#ifdef QT_QTXML
#include <QtXml/QtXml>
#endif
#ifdef QT_QTSQL
#include <QtSql/QtSql>
#endif
#include <QtOpenGL/QtOpenGL>
#ifdef QT_QTOPENGL
#include <QtNetwork/QtNetwork>
#endif
#ifdef QT_QTSVG
#include <QtSvg/QtSvg>
#endif

// some MS headers do
// #define interface struct
// un-define it here so QtDBus will build correctly
#ifdef QT_QTDBUS
#undef interface
#include <QtDBus/QtDBus>
#endif
