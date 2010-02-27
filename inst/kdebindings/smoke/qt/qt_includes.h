#include <QtCore/QtCore>
#ifndef QT_NO_GUI
#include <QtGui/QtGui>
#endif

#ifdef Q_WS_X11
#include <QtGui/qx11info_x11.h>
#include <QtGui/qx11embed_x11.h>
#endif

#ifndef QT_NO_XML
#include <QtXml/QtXml>
#endif
#ifndef QT_NO_SQL
#include <QtSql/QtSql>
#endif
#ifndef QT_NO_OPENGL
#include <QtOpenGL/QtOpenGL>
#endif
#ifndef QT_NO_NETWORK
#include <QtNetwork/QtNetwork>
#endif
#ifndef QT_NO_SVG
#include <QtSvg/QtSvg>
#endif

// some MS headers do
// #define interface struct
// un-define it here so QtDBus will build correctly
#ifndef QT_NO_DBUS
#undef interface
#include <QtDBus/QtDBus>
#endif
