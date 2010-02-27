#include <QtCore/QtCore>
#ifdef QT_GUI
#include <QtGui/QtGui>
#endif

#ifdef Q_WS_X11
#include <QtGui/qx11info_x11.h>
#include <QtGui/qx11embed_x11.h>
#endif

#ifdef QT_XML
#include <QtXml/QtXml>
#endif
#ifdef QT_SQL
#include <QtSql/QtSql>
#endif
#ifdef QT_OPENGL
#include <QtOpenGL/QtOpenGL>
#endif
#ifdef QT_NETWORK
#include <QtNetwork/QtNetwork>
#endif
#ifdef QT_SVG
#include <QtSvg/QtSvg>
#endif

// some MS headers do
// #define interface struct
// un-define it here so QtDBus will build correctly
#ifdef QT_DBUS
#undef interface
#include <QtDBus/QtDBus>
#endif
