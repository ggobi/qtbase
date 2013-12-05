#include <QtCore/QtCore>
#include <QtGui/QtGui>
#include <QtWidgets/QtWidgets>

#ifdef Q_WS_X11
#include <QtGui/qx11info_x11.h>
#include <QtGui/qx11embed_x11.h>
#endif

#ifdef QT_MULTIMEDIA_LIB
#include <QtMultimedia/QtMultimedia>
#endif
#ifdef QT_NETWORK_LIB
#include <QtNetwork/QtNetwork>
#endif
#ifdef QT_QML_LIB
#include <QtQml/QtQml>
#endif
#ifdef QT_QUICK_LIB
#include <QtQuick/QtQuick>
#endif
#ifdef QT_SQL_LIB
#include <QtSql/QtSql>
#endif
#if defined(QT_TEST_LIB) || defined(QT_TESTLIB_LIB)
#include <QtTest/QtTest>
#endif
#ifdef QT_WEBKIT_LIB
#include <QtWebKit/QtWebKit>
#endif
#ifdef QT_WEBKITWIDGETS_LIB
#include <QtWebKitWidgets/QtWebKitWidgets>
#endif
#ifdef QT_SVG_LIB
#include <QtSvg/QtSvg>
#endif
#ifdef QT_XMLPATTERNS_LIB
#include <QtXmlPatterns/QtXmlPatterns>
#endif
#ifdef QT_OPENGL_LIB
#include <QtOpenGL/QtOpenGL>
#endif
#ifdef QT_PRINTSUPPORT_LIB
#include <QtPrintSupport/QtPrintSupport>
#endif
#ifdef QT_HELP_LIB
#include <QtHelp/QtHelp>
#endif
#ifdef QT_UITOOLS_LIB
#include <QtUiTools/QtUiTools>
#endif
#ifdef QT_SENSORS_LIB // untested
#include <QtSensors/QtSensors>
#endif
#ifdef QT_SERIALPORT_LIB // untested
#include <QtSerialPort/QtSerialPort>
#endif
#ifdef QT_BLUETOOTH_LIB // untested
#include <QtBluetooth/QtBluetooth>
#endif
#ifdef QT_POSITIONING_LIB // untested
#include <QtPositioning/QtPositioning>
#endif
#ifdef QT_NFC_LIB // untested
#include <QtNfc/QtNfc>
#endif

// some MS headers do
// #define interface struct
// un-define it here so QtDBus will build correctly
#ifdef QT_DBUS_LIB
#undef interface
#include <QtDBus/QtDBus>
#endif
