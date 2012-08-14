cmake_minimum_required(VERSION 2.8.1)

project(qtbase)

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

## QtUiTools not linked correctly on Windows (_personality_v0 missing symbol)
if(WIN32)
set(QT_USE_QTUITOOLS FALSE)
endif(WIN32)

include(${QT_USE_FILE})

include_directories( ../inst/local/include ${R_INCLUDES} )

link_directories( ../kdebindings-build/smoke/qt )

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
   
QT4_WRAP_CPP ( qtbase_MOC_FILES EventLoop.hpp DataFrameModel.hpp )

if(WIN32) # Toughest Win32 part: generating the defs file for the DLL
foreach(qtbase_lib_src ${qtbase_LIB_SRCS})
  list(APPEND qtbase_LIB_OBJS "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/qtbase.dir/${qtbase_lib_src}${CMAKE_CXX_OUTPUT_EXTENSION}")
endforeach(qtbase_lib_src ${qtbase_LIB_SRCS})
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/qtbase.def 
                   COMMAND sh ARGS ${CMAKE_CURRENT_SOURCE_DIR}/mkdef.sh
                   DEPENDS ${qtbase_LIB_OBJS}) 
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

INCLUDE(CMakeForceCompiler)
CMAKE_FORCE_CXX_COMPILER(${R_CXX} R_CXX)

