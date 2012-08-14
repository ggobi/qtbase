cmake_minimum_required(VERSION 2.6)

project(kdebindings)

find_package(Qt4 4.0.0 REQUIRED COMPONENTS QtCore QtGui)

if(APPLE) # No QtDBus in Mac binaries
find_package(Qt4 4.0.0 COMPONENTS QtXml QtXmlPatterns QtNetwork
             QtOpenGL QtSql QtSvg QtScript QtWebkit QtTest
             QtHelp QtMultimedia QtDeclarative QtUiTools)
else(APPLE)
find_package(Qt4 4.0.0 COMPONENTS QtXml QtXmlPatterns QtNetwork
             QtOpenGL QtSql QtSvg QtDBus QtScript QtWebkit QtTest
             QtHelp QtMultimedia QtDeclarative QtUiTools)
endif(APPLE)

## FindQt4 does not add the include directory for OpenGL
find_package(OpenGL)
if(OPENGL_FOUND)
include_directories(${OPENGL_INCLUDE_DIR})
endif(OPENGL_FOUND)

## QtUiTools not installed as a framework on OS X
if(APPLE)
set(QT_USE_QTUITOOLS FALSE)
endif(APPLE)

## QtUiTools not linked correctly on Windows (_personality_v0 missing symbol)
if(WIN32)
set(QT_USE_QTUITOOLS FALSE)
endif(WIN32)

include(${QT_USE_FILE})

add_subdirectory(generator)
add_subdirectory(smoke)
