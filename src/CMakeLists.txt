cmake_minimum_required(VERSION 2.8.1)

project(qtbase)

find_package(Qt4 4.0.0 COMPONENTS QtCore QtGui QtXml QtXmlPatterns
  QtNetwork QtOpenGL QtSql QtSvg QtDBus QtScript QtWebkit
  QtTest QtHelp QtMultimedia QtDeclarative REQUIRED )

include(${QT_USE_FILE})

include_directories( ../inst/local/include ${R_INCLUDES} )

link_directories( ../inst/local/lib )

set(qtbase_LIB_SRCS init.cpp EventLoop.cpp convert.cpp
   wrap.cpp metaobject.cpp connect.cpp invoke.cpp dynamicqobject.cpp
   RDynamicQObject.cpp ClassFactory.cpp Class.cpp SmokeClass.cpp
   MocClass.cpp RClass.cpp classes.cpp ForeignMethod.cpp
   SmokeMethod.cpp RMethod.cpp MocMethod.cpp DynamicBinding.cpp
   MocDynamicBinding.cpp MethodCall.cpp type-handlers.cpp MocStack.cpp
   MocProperty.cpp RProperty.cpp SmokeModule.cpp module.cpp RSmokeBinding.cpp
   SmokeList.cpp SmokeObject.cpp ObjectTable.cpp
   InstanceObjectTable.cpp smoke.cpp DataFrameModel.cpp
   RTextFormattingDelegate.cpp)

QT4_WRAP_CPP ( qtbase_MOC_FILES EventLoop.hpp )

if(WIN32) # Toughest Win32 part: generating the defs file for the DLL
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/qtbase.def 
                   COMMAND sh ARGS ${CMAKE_CURRENT_SOURCE_DIR}/mkdef.sh
                   DEPENDS ${qtbase_LIB_SRCS})
SET(CMAKE_LINK_DEF_FILE_FLAG "qtbase.def ") # space needs to be there
add_library(qtbase SHARED qtbase.def ${qtbase_LIB_SRCS} ${qtbase_MOC_FILES})
else(WIN32)
add_library(qtbase SHARED ${qtbase_LIB_SRCS} ${qtbase_MOC_FILES})
endif(WIN32)

set_target_properties(qtbase PROPERTIES COMPILE_FLAGS "-ggdb")

target_link_libraries(qtbase smokeqt ${QT_LIBRARIES} ${R_LIBRARIES})

set_target_properties(qtbase PROPERTIES PREFIX "")
if(NOT WIN32) # Need to force .so on Mac, but Windows needs to be .dll
set_target_properties(qtbase PROPERTIES SUFFIX ".so")
install(TARGETS qtbase LIBRARY DESTINATION . )
else(NOT WIN32)
install(TARGETS qtbase RUNTIME DESTINATION . )
endif(NOT WIN32)


