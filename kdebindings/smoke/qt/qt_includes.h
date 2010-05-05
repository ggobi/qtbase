#include <QtCore/QtCore>
#include <QtGui/QtGui>

#ifdef Q_WS_X11
#include <QtGui/qx11info_x11.h>
#include <QtGui/qx11embed_x11.h>
#endif

#ifdef QT_XML_LIB
#include <QtXml/QtXml>
#endif
#ifdef QT_XMLPATTERNS_LIB
#include <QtXmlPatterns/QtXmlPatterns>
#endif
#ifdef QT_SQL_LIB
#include <QtSql/QtSql>
#endif
#ifdef QT_OPENGL_LIB
#include <QtOpenGL/QtOpenGL>
#endif
#ifdef QT_NETWORK_LIB
#include <QtNetwork/QtNetwork>
#endif
#ifdef QT_SVG_LIB
#include <QtSvg/QtSvg>
#endif
#ifdef QT_SCRIPT_LIB
#include <QtScript/QtScript>
#endif
#ifdef QT_TEST_LIB
#include <QtTest/QtTest>
#endif
#ifdef QT_WEBKIT_LIB
#include <QtWebKit/QtWebKit>
#endif
#ifdef QT_HELP_LIB
#include <QtHelp/QtHelp>
#endif
#ifdef QT_MULTIMEDIA_LIB
#include <QtMultimedia/QtMultimedia>
#endif

// some MS headers do
// #define interface struct
// un-define it here so QtDBus will build correctly
#ifdef QT_DBUS_LIB
#undef interface
#include <QtDBus/QtDBus>
#endif
