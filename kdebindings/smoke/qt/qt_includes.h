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
#ifdef QT_PHONON_LIB
#define PHONON_EXPORT
#include <phonon/abstractaudiooutput.h>
#include <phonon/abstractmediastream.h>
#include <phonon/abstractvideooutput.h>
#include <phonon/addoninterface.h>
#include <phonon/audiodataoutput.h>
#include <phonon/audiodataoutputinterface.h>
#include <phonon/audiooutput.h>
#include <phonon/audiooutputinterface.h>
#include <phonon/backendcapabilities.h>
#include <phonon/backendinterface.h>
#include <phonon/effect.h>
#include <phonon/effectinterface.h>
#include <phonon/effectparameter.h>
#include <phonon/effectwidget.h>
#include <phonon/mediacontroller.h>
#include <phonon/medianode.h>
#include <phonon/mediaobject.h>
#include <phonon/mediaobjectinterface.h>
#include <phonon/mediasource.h>
#include <phonon/objectdescription.h>
#include <phonon/objectdescriptionmodel.h>
#include <phonon/path.h>
#include <phonon/phonon_export.h>
#include <phonon/phonondefs.h>
#include <phonon/phononnamespace.h>
#include <phonon/platformplugin.h>
#include <phonon/seekslider.h>
#include <phonon/streaminterface.h>
#include <phonon/videoplayer.h>
#include <phonon/videowidget.h>
#include <phonon/videowidgetinterface.h>
#include <phonon/volumefadereffect.h>
#include <phonon/volumefaderinterface.h>
#include <phonon/volumeslider.h>
#endif

// some MS headers do
// #define interface struct
// un-define it here so QtDBus will build correctly
#ifdef QT_DBUS_LIB
#undef interface
#include <QtDBus/QtDBus>
#endif
