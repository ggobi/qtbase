#include <QtCore/QtCore>
#include <QtGui/QtGui>
#include <QtWidgets/QtWidgets>

#if QT_VERSION >= 0x50900
#include <QFloat16>
#endif

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
#ifdef QT_QUICK_WIDGETS_LIB
#include <QtQuickWidgets/QtQuickWidgets>
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
#ifdef QT_WEBSOCKETS_LIB // untested
#include <QtWebSockets/QtWebSockets>
#endif
#ifdef QT_WEBENGINE_LIB // untested
#include <QtWebEngine/QtWebEngine>
#endif
#ifdef QT_WEBCHANNEL_LIB // untested
#include <QtWebChannel/QtWebChannel>
#endif
#ifdef QT_WEBENGINECORE_LIB // untested
#include <QtWebEngineCore/QtWebEngineCore>
#endif
#ifdef QT_WEBENGINEWIDGETS_LIB // untested
#include <QtWebEngineWidgets/QtWebEngineWidgets>
#endif
#ifdef QT_3DANIMATION_LIB // untested
#include <Qt3DAnimation/Qt3DAnimation>
#endif
#ifdef QT_3DCORE_LIB // untested
#include <Qt3DCore/Qt3DCore>
#endif
#ifdef QT_3DEXTRAS_LIB // untested
#include <Qt3DExtras/Qt3DExtras>
#endif
#ifdef QT_3DINPUT_LIB // untested
#include <Qt3DInput/Qt3DInput>
#endif
#ifdef QT_3DLOGICL_LIB // untested
#include <Qt3DLogic/Qt3DLogic>
#endif
#ifdef QT_3DRENDER_LIB // untested
#include <Qt3DRender/Qt3DRender>
#endif
#ifdef QT_CHARTS_LIB // untested
#include <QtCharts/QtCharts>
#endif
#ifdef QT_DATAVISUALIZATION_LIB // untested
#include <QtDataVisualization/QtDataVisualization>
#endif
#ifdef QT_LOCATION_LIB // untested
#include <QtLocation/QtLocation>
#endif
#ifdef QT_SCXML_LIB // untested
#include <QtScxml/QtScxml>
#endif
#ifdef QT_SERIALBUS_LIB // untested
#include <QtSerialBus/QtSerialBus>
#endif
#ifdef QT_GAMEPAD_LIB // untested
#include <QtGamepad/QtGamepad>
#endif
#ifdef QT_NETWORKAUTH_LIB // untested
#include <QtNetworkAuth/QtNetworkAuth>
#endif
#ifdef QT_TEXTOSPEECH_LIB // untested
#include <QtTextToSpeech/QtTextToSpeech>
#endif

// some MS headers do
// #define interface struct
// un-define it here so QtDBus will build correctly
#ifdef QT_DBUS_LIB
#undef interface
#include <QtDBus/QtDBus>
#endif
