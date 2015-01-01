#ifdef TEST_QT_NO_DEBUG
#include "QtWidgets/qgraphicslinearlayout.h"

int main(int argc, char ** argv)
{
  ((QGraphicsLinearLayout*)0)->dump();
}

#endif

#ifdef TEST_QT_NO_ACCESSIBILITY
#include "QtGui/qaccessible.h"
#include "QtGui/qaccessiblebridge.h"

int main(int argc, char ** argv)
{
  QAccessibleEvent event(NULL, QAccessible::SoundPlayed);
  const QMetaObject& mo = QAccessibleBridgePlugin::staticMetaObject;
}

#endif

#ifdef TEST_QT_NO_ACTION
#include "QtWidgets/qaction.h"

int main(int argc, char ** argv)
{
  QAction foo( (QObject*)NULL );
}

#endif

#ifdef TEST_QT_NO_BUTTONGROUP
#include "QtWidgets/qbuttongroup.h"

int main(int argc, char ** argv)
{
  QButtonGroup foo( (QObject*)NULL );
}

#endif

#ifdef TEST_QT_NO_CHECKBOX
#include "QtWidgets/qcheckbox.h"

int main(int argc, char ** argv)
{
  QCheckBox foo( (QWidget*)NULL );
}

#endif

#ifdef TEST_QT_NO_CLIPBOARD
#include "QtWidgets/qapplication.h"
#include "QtWidgets/qclipboard.h"

int main(int argc, char ** argv)
{
  QApplication foo( argc, argv );
  QClipboard *baz= foo.clipboard();
}

#endif

#ifdef TEST_QT_NO_COLORDIALOG
#include "QtWidgets/qcolordialog.h"

int main(int argc, char ** argv)
{
  QColorDialog::customCount();
}

#endif

#ifdef TEST_QT_NO_COMBOBOX
#include "QtWidgets/qcombobox.h"

int main(int argc, char ** argv)
{
  QComboBox foo( (QWidget*)NULL );
}

#endif

#ifdef TEST_QT_NO_COMPAT
#include "QtWidgets/qfontmetrics.h"

int main(int argc, char ** argv)
{
  QFontMetrics *foo= new QFontMetrics( QFont() );
  int bar = foo->width( 'c' );
}

#endif

#ifdef TEST_QT_NO_COMPONENT
#include "QtWidgets/qapplication.h"

int main(int argc, char ** argv)
{
  QApplication foo( argc, argv );
  foo.addLibraryPath( QString::null );
}

#endif

#ifdef TEST_QT_NO_CURSOR
#include "QtWidgets/qcursor.h"

int main(int argc, char ** argv)
{
  QCursor foo;
}

#endif

#ifdef TEST_QT_NO_DATASTREAM
#include "QtCore/qdatastream.h"

int main(int argc, char ** argv)
{
  QDataStream foo;
}

#endif

#ifdef TEST_QT_NO_DATETIMEEDIT
#include "QtWidgets/qdatetimeedit.h"

int main(int argc, char ** argv)
{
  QTimeEdit foo;
}

#endif

#ifdef TEST_QT_NO_DIAL
#include "QtWidgets/qdial.h"

int main(int argc, char ** argv)
{
  QDial foo;
}

#endif

#ifdef TEST_QT_NO_DIALOG
#include "QtWidgets/qdialog.h"

int main(int argc, char ** argv)
{
  QDialog foo;
}

#endif

#ifdef TEST_QT_NO_DIR
#include "QtCore/qdir.h"

int main(int argc, char ** argv)
{
  QDir foo;
}

#endif

#ifdef TEST_QT_NO_DOM
#include "QtXml/qdom.h"

int main(int argc, char ** argv)
{
  QDomDocumentType foo;
}

#endif

#ifdef TEST_QT_NO_DRAGANDDROP
#include "QtWidgets/qevent.h"

int main(int argc, char ** argv)
{
  QDropEvent foo( QDropEvent() );
}

#endif

#ifdef TEST_QT_NO_DRAWUTIL
#include "QtWidgets/qdrawutil.h"
#include "QtWidgets/qcolor.h"

int main(int argc, char ** argv)
{
  qDrawPlainRect( (QPainter *) NULL, 0, 0, 0, 0, QColor() );
}

#endif

#ifdef TEST_QT_NO_ERRORMESSAGE
#include "QtWidgets/qerrormessage.h"

int main(int argc, char ** argv)
{
  QErrorMessage foo( (QWidget*) NULL );
}

#endif

#ifdef TEST_QT_NO_FILEDIALOG
#include "QtWidgets/qfiledialog.h"

int main(int argc, char ** argv)
{
  QFileDialog foo();
}


#endif

#ifdef TEST_QT_NO_FONTDATABASE
#include "QtWidgets/qfontdatabase.h"

int main(int argc, char ** argv)
{
  QFontDatabase foo;
}

#endif

#ifdef TEST_QT_NO_FONTDIALOG
#include "QtWidgets/qfontdialog.h"

int main(int argc, char ** argv)
{
  QFontDialog::getFont( (bool *)NULL );
}

#endif

#ifdef TEST_QT_NO_FRAME
#include "QtWidgets/qframe.h"

int main(int argc, char ** argv)
{
  QFrame foo;
}

#endif

#ifdef TEST_QT_NO_FTP
#include "QtNetwork/qftp.h"

int main(int argc, char ** argv)
{
  QFtp foo;
}

#endif

#ifdef TEST_QT_NO_GROUPBOX
#include "QtWidgets/qgroupbox.h"

int main(int argc, char ** argv)
{
  QGroupBox foo;
}

#endif

#ifdef TEST_QT_NO_IMAGE_HEURISTIC_MASK
#include "QtWidgets/qimage.h"

int main(int argc, char ** argv)
{
  QImage *foo = new QImage;
  foo->createHeuristicMask();
}

#endif

#ifdef TEST_QT_NO_IMAGEIO
#include "QtWidgets/qbitmap.h"
#include "QtCore/qstring.h"

int main(int argc, char ** argv)
{
  QBitmap foo( QString::fromLatin1('foobar') );
}

#endif

#ifdef TEST_QT_NO_LABEL
#include "QtWidgets/qlabel.h"

int main(int argc, char ** argv)
{
  QLabel foo( (QWidget*) NULL );
}

#endif

#ifdef TEST_QT_NO_LAYOUT
#include "QtWidgets/qlayout.h"

class QFoo: public QLayout
{
public:
    QFoo() {};
    ~QFoo() {};
    void addItem( QLayoutItem * ) { };
    QSize sizeHint() const { return QSize(); }
    void setGeometry( const QRect & ) { };
                QLayoutItem* itemAt(int) const {return (QLayoutItem*) NULL;}
                QLayoutItem* takeAt(int) {return (QLayoutItem*) NULL;}
                int count() const {return 0;}

};

int main(int argc, char ** argv)
{
  QFoo foo;
}

#endif

#ifdef TEST_QT_NO_LCDNUMBER
#include "QtWidgets/qlcdnumber.h"

int main(int argc, char ** argv)
{
  QLCDNumber foo;
}

#endif

#ifdef TEST_QT_NO_LINEEDIT
#include "QtWidgets/qlineedit.h"

int main(int argc, char ** argv)
{
  QLineEdit foo( (QWidget *) NULL );
}

#endif

#ifdef TEST_QT_NO_LISTVIEW
#include "QtWidgets/qlistview.h"

int main(int argc, char ** argv)
{
  QListView foo;
}

#endif

#ifdef TEST_QT_NO_MAINWINDOW
#include "QtWidgets/qmainwindow.h"

int main(int argc, char ** argv)
{
  QMainWindow foo;
}

#endif

#ifdef TEST_QT_NO_MATRIX
#include "QtWidgets/qmatrix.h"

int main(int argc, char ** argv)
{
  QMatrix foo;
}

#endif

#ifdef TEST_QT_NO_MENUBAR
#include "QtWidgets/qmenubar.h"

int main(int argc, char ** argv)
{
  QMenuBar foo;
}

#endif

#ifdef TEST_QT_NO_MESSAGEBOX
#include "QtWidgets/qmessagebox.h"

int main(int argc, char ** argv)
{
  QMessageBox foo;
}

#endif

#ifdef TEST_QT_NO_MOVIE
#include "QtWidgets/qmovie.h"

int main(int argc, char ** argv)
{
  QMovie foo;
}

#endif

#ifdef TEST_QT_NO_NETWORK
#include "QtNetwork/qnetworkproxy.h"

int main(int argc, char ** argv)
{
  QNetworkProxy foo;
}

#endif

#ifdef TEST_QT_NO_PALETTE
#include "QtWidgets/qpalette.h"

int main(int argc, char ** argv)
{
  QPalette foo;
}

#endif

#ifdef TEST_QT_NO_PICTURE
#include "QtWidgets/qpicture.h"

int main(int argc, char ** argv)
{
  QPicture foo;
}

#endif

#ifdef TEST_QT_NO_PRINTER
#include "QtPrintSupport/qprinter.h"

int main(int argc, char ** argv)
{
  QPrinter foo();
}

#endif

#ifdef TEST_QT_NO_PRINTDIALOG
#include "QtPrintSupport/qprintdialog.h"

int main(int argc, char ** argv)
{
  QPrintDialog foo( (QPrinter*) NULL );
}

#endif

#ifdef TEST_QT_NO_PROCESS
#include "QtCore/qprocess.h"

int main(int argc, char ** argv)
{
  QProcess foo;
}

#endif

#ifdef TEST_QT_NO_PROGRESSBAR
#include "QtWidgets/qprogressbar.h"

int main(int argc, char ** argv)
{
  QProgressBar foo;
}

#endif

#ifdef TEST_QT_NO_PROGRESSDIALOG
#include "QtWidgets/qprogressdialog.h"

int main(int argc, char ** argv)
{
  QProgressDialog foo;
}

#endif

#ifdef TEST_QT_NO_PROPERTIES
#include "QtCore/qmetaobject.h"

int main(int argc, char ** argv)
{
  QMetaProperty foo;
}

#endif

#ifdef TEST_QT_NO_PUSHBUTTON
#include "QtWidgets/qpushbutton.h"

int main(int argc, char ** argv)
{
  QPushButton foo( (QWidget *) NULL );
}

#endif

#ifdef TEST_QT_NO_QUUID_STRING
#include "QtCore/quuid.h"

int main(int argc, char ** argv)
{
  QUuid foo( QString() );
}

#endif

#ifdef TEST_QT_NO_RADIOBUTTON
#include "QtWidgets/qradiobutton.h"

int main(int argc, char ** argv)
{
  QRadioButton foo( (QWidget *) NULL );
}

#endif

#ifdef TEST_QT_NO_SCROLLBAR
#include "QtWidgets/qscrollbar.h"

int main(int argc, char ** argv)
{
  QScrollBar foo( (QWidget *) NULL );
}

#endif

#ifdef TEST_QT_NO_SESSIONMANAGER
#include "QtWidgets/qapplication.h"

int main(int argc, char ** argv)
{
  QApplication foo( argc, argv );
  foo.sessionId();
}

#endif

#ifdef TEST_QT_NO_SETTINGS
#include "QtCore/qsettings.h"

int main(int argc, char ** argv)
{
  QSettings foo;
}

#endif

#ifdef TEST_QT_NO_SIGNALMAPPER
#include "QtCore/qsignalmapper.h"

int main(int argc, char ** argv)
{
  QSignalMapper foo( (QObject *) NULL );
}

#endif

#ifdef TEST_QT_NO_SIZEGRIP
#include "QtWidgets/qsizegrip.h"

int main(int argc, char ** argv)
{
  QSizeGrip foo( (QWidget *) NULL );
}

#endif

#ifdef TEST_QT_NO_SLIDER
#include "QtWidgets/qslider.h"

int main(int argc, char ** argv)
{
  QSlider foo( (QWidget *) NULL );
}

#endif

#ifdef TEST_QT_NO_SOUND
#include "QtWidgets/qsound.h"

int main(int argc, char ** argv)
{
  QSound foo( QString::null );
}

#endif

#ifdef TEST_QT_NO_STRINGLIST
#include "qstringlist.h"

int main(int argc, char ** argv)
{
  QStringList foo;
}

#endif

#ifdef TEST_QT_NO_STYLE_PLASTIQUE
#include "QtWidgets/qplastiquestyle.h"

int main(int argc, char ** argv)
{
  QPlastiqueStyle foo;
}

#endif

#ifdef TEST_QT_NO_STYLE_WINDOWSXP
#include "QtWidgets/qwindowsxpstyle.h"

int main(int argc, char ** argv)
{
  QWindowsXPStyle foo;
}

#endif

#ifdef TEST_QT_NO_STYLE_WINDOWS
#include "QtWidgets/qwindowsstyle.h"

int main(int argc, char ** argv)
{
  QWindowsStyle foo;
}

#endif

#ifdef TEST_QT_NO_STYLE_MAC
#include "QtWidgets/macstyle.h"

int main(int argc, char ** argv)
{
  QMacStyle foo;
}

#endif

#ifdef TEST_QT_NO_STYLE_CLEANLOOKS
#include "QtWidgets/qcleanlooksstyle.h"

int main(int argc, char ** argv)
{
  QCleanlooksStyle foo;
}

#endif

#ifdef TEST_QT_NO_TABBAR
#include "QtWidgets/qtabbar.h"

int main(int argc, char ** argv)
{
  QTabBar foo;
}

#endif

#ifdef TEST_QT_NO_TABLEVIEW
#include "QtWidgets/qtableview.h"

int main(int argc, char ** argv)
{
  QTableView foo;
}

#endif

#ifdef TEST_QT_NO_TABWIDGET
#include "QtWidgets/qtabwidget.h"

int main(int argc, char ** argv)
{
  QTabWidget foo;
}

#endif

#ifdef TEST_QT_NO_TEXTBROWSER
#include "QtWidgets/qtextbrowser.h"

int main(int argc, char ** argv)
{
  QTextBrowser foo;
}

#endif

#ifdef TEST_QT_NO_TEXTCODEC
#include "QtCore/qtextcodec.h"

int main(int argc, char ** argv)
{
  QTextCodec::codecForMib(1);
}

#endif

#ifdef TEST_QT_NO_TEXTCODECPLUGIN
#include "QtCore/qtextcodecplugin.h"
#include "QtCore/qstringlist.h"
#include "QtCore/qlist.h"
#include "QtCore/qtextcodec.h"

class QFoo: public QTextCodecPlugin
{
public:
    QFoo() {};
    ~QFoo() {};
                QList<QByteArray> names() const {return QList<QByteArray>();}
                QList<int>mibEnums() const {return QList<int>();}
                QTextCodec *createForName( const QByteArray & name ) {return (QTextCodec *)NULL;}
                QTextCodec *createForMib( int mib ) {return (QTextCodec *)NULL;}
                QList<QByteArray> aliases() const {return QList<QByteArray>();}
};
Q_EXPORT_PLUGIN2( 'Foo', QFoo )

int main(int argc, char ** argv)
{
  QFoo foo;
}

#endif

#ifdef TEST_QT_NO_TEXTEDIT
#include "QtWidgets/qtextedit.h"

int main(int argc, char ** argv)
{
  QTextEdit foo; 
}

#endif

#ifdef TEST_QT_NO_TEXTSTREAM
#include "QtCore/qtextstream.h"

int main(int argc, char ** argv)
{
  QTextStream foo;
}

#endif

#ifdef TEST_QT_NO_THREAD
#include "QtCore/qthread.h"
class QFoo: public QThread
{
public:
    QFoo() {};
    ~QFoo() {};
                void run() {}
};

int main(int argc, char ** argv)
{
  QFoo foo;
}


#endif

#ifdef TEST_QT_NO_TOOLBAR
#include "QtWidgets/qtoolbar.h"

int main(int argc, char ** argv)
{
  QToolBar foo;
}

#endif

#ifdef TEST_QT_NO_TOOLBUTTON
#include "QtWidgets/qtoolbutton.h"

int main(int argc, char ** argv)
{
  QToolButton foo((QWidget *) NULL );
}

#endif

#ifdef TEST_QT_NO_TOOLTIP
#include "QtWidgets/qtooltip.h"

int main(int argc, char ** argv)
{
  QToolTip::palette();
}

#endif

#ifdef TEST_QT_NO_VARIANT
#include "QtCore/qvariant.h"

int main(int argc, char ** argv)
{
  QVariant foo;
}

#endif

#ifdef TEST_QT_NO_WHATSTHIS
#include "QtWidgets/qwhatsthis.h"

int main(int argc, char ** argv)
{
  QWhatsThis::inWhatsThisMode();
}

#endif

#ifdef TEST_QT_NO_WHEELEVENT
#include "QtWidgets/qevent.h"

int main(int argc, char ** argv)
{
  QWheelEvent foo( QPoint(1,1), 1, (Qt::MouseButtons)1, 0 );
}

#endif
