cmake_minimum_required(VERSION 2.8.1)

project(qtbase)

find_package(Qt4 4.0.0 COMPONENTS QtCore QtGui QtXml QtXmlPatterns
  QtNetwork QtOpenGL QtSql QtSvg QtDBus QtScript QtWebkit
  QtTest QtHelp phonon REQUIRED )

include(${QT_USE_FILE})

include_directories( ../inst/local/include ${R_INCLUDES} )

link_directories( ../inst/local/lib )

set(qtbase_LIB_SRCS init.cpp EventLoop.cpp style.cpp convert.cpp
   wrap.cpp metaobject.cpp connect.cpp invoke.cpp dynamicqobject.cpp
   RDynamicQObject.cpp ClassFactory.cpp Class.cpp SmokeClass.cpp
   MocClass.cpp RClass.cpp classes.cpp ForeignMethod.cpp
   SmokeMethod.cpp RMethod.cpp MocMethod.cpp DynamicBinding.cpp
   MocDynamicBinding.cpp MethodCall.cpp type-handlers.cpp MocStack.cpp
   MocProperty.cpp RQtModule.cpp module.cpp RSmokeBinding.cpp
   SmokeList.cpp SmokeObject.cpp ObjectTable.cpp
   InstanceObjectTable.cpp smoke.cpp DataFrameModel.cpp )

QT4_WRAP_CPP ( qtbase_MOC_FILES EventLoop.hpp )

add_library(qtbase SHARED ${qtbase_LIB_SRCS} ${qtbase_MOC_FILES})

set_target_properties(qtbase PROPERTIES COMPILE_FLAGS "-ggdb")

target_link_libraries(qtbase smokeqt ${QT_LIBRARIES} ${R_LIBRARIES})

set_target_properties(qtbase PROPERTIES PREFIX "")
set_target_properties(qtbase PROPERTIES SUFFIX ".so")

install(TARGETS qtbase DESTINATION . )
