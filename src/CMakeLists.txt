cmake_minimum_required(VERSION 2.8)

find_package(Qt4 4.0.0 COMPONENTS QtCore QtGui QtXml QtXmlPatterns
  QtAssistant QtNetwork QtOpenGL QtSql QtSvg QtDBus QtScript QtWebkit
  QtPhonon QtTest QtUiTools REQUIRED )

include_directories( ../local/include ${R_INCLUDES} )
include(${QT_USE_FILE})

link_directories( ../local/lib )

set(qtbase_LIB_SRCS init.cpp EventLoop.cpp style.cpp convert.cpp
   wrap.cpp metaobject.cpp connect.cpp invoke.cpp dynamicqobject.cpp
   RDynamicQObject.cpp ClassFactory.cpp Class.cpp SmokeClass.cpp
   MocClass.cpp RClass.cpp classes.cpp ForeignMethod.cpp
   SmokeMethod.cpp RMethod.cpp MocMethod.cpp DynamicBinding.cpp
   MocDynamicBinding.cpp MethodCall.cpp type-handlers.cpp MocStack.cpp
   MocProperty.cpp RQtModule.cpp module.cpp RSmokeBinding.cpp
   SmokeList.cpp SmokeObject.cpp ObjectTable.cpp
   InstanceObjectTable.cpp smoke.cpp )

QT4_AUTOMOC ( ${qtbase_LIB_SRCS} )

add_library(qtbase SHARED ${qtbase_LIB_SRCS})

target_link_libraries(qtbase smokeqt ${QT_LIBRARIES} ${R_LIBRARIES})

set_target_properties(qtbase PROPERTIES PREFIX "")
