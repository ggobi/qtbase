#include <QtCore/QtCore>
#ifdef QT_QTGUI
#include <QtGui/QtGui>
#endif

#ifdef Q_WS_X11
#include <QtGui/qx11info_x11.h>
#include <QtGui/qx11embed_x11.h>
#endif

#ifdef QT_QTXML
#include <QtXml/QtXml>
#endif
#ifdef QT_QTSQL
#include <QtSql/QtSql>
#endif
#ifdef QT_QTOPENGL
#include <QtOpenGL/QtOpenGL>
#endif
#ifdef QT_QTNETWORK
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
