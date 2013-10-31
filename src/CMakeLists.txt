cmake_minimum_required(VERSION 2.8.8)

project(qtbase)

find_package(Qt5Core)

set(CMAKE_AUTOMOC ON)

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

if(WIN32) # Toughest Win32 part: generating the defs file for the DLL
foreach(qtbase_lib_src ${qtbase_LIB_SRCS})
  list(APPEND qtbase_LIB_OBJS "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/qtbase.dir/${qtbase_lib_src}${CMAKE_CXX_OUTPUT_EXTENSION}")
endforeach(qtbase_lib_src ${qtbase_LIB_SRCS})
if (CMAKE_SIZEOF_VOID_P EQUAL 4)
  set(SYMPAT "'s/^.* [BCDRT] _/ /p'")
else ()
  set(SYMPAT "/^[^ ]* [T|C|B] /s/^[^ ]* [T|C|B] / /p")
endif()
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/qtbase.def 
                   COMMAND sh
                   ARGS ${CMAKE_CURRENT_SOURCE_DIR}/mkdef.sh ${SYMPAT}
                   DEPENDS ${qtbase_LIB_OBJS}) 
SET(CMAKE_LINK_DEF_FILE_FLAG "qtbase.def ") # space needs to be there
add_library(qtbase SHARED qtbase.def ${qtbase_LIB_SRCS} ${qtbase_MOC_FILES})
else(WIN32)
add_library(qtbase SHARED ${qtbase_LIB_SRCS} ${qtbase_MOC_FILES})
endif(WIN32)

qt5_use_modules(qtbase Widgets)

macro(qt5_optional_use_modules _target)
  foreach(_module ${ARGN})
    find_package(Qt5${_module} QUIET)
    if(Qt5${_module}_FOUND)
      qt5_use_modules(${_target} ${_module})
      message(STATUS "Found Qt module: ${_module}")
    endif()
  endforeach()
endmacro()

## new in 5.2
qt5_optional_use_modules(qtbase Multimedia MultimediaWidgets Network Qml Quick
                         Sql Test Webkit WebKitWidgets DBus Svg XmlPatterns
                         PrintSupport Help UiTools SerialPort Sensors
                         Bluetooth Positioning)

## QtUiTools not linked correctly on Windows (_personality_v0 missing symbol)
if(WIN32)
set(QT_USE_QTUITOOLS FALSE)
endif(WIN32)

##set_target_properties(qtbase PROPERTIES COMPILE_FLAGS "-ggdb")

target_link_libraries(qtbase smokeqt ${R_LIBRARIES})

## Seems GCC does not consider the dependencies of smokeqt.a, so it
## skips linkages that are not required by the object files.
## Thus, we disable the skipping.
string(REPLACE "-Wl,--as-needed" ""
       CMAKE_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS})

set_target_properties(qtbase PROPERTIES PREFIX "")
if(NOT WIN32) # Need to force .so on Mac, but Windows needs to be .dll
set_target_properties(qtbase PROPERTIES SUFFIX ".so")
install(TARGETS qtbase LIBRARY DESTINATION . )
else(NOT WIN32)
install(TARGETS qtbase RUNTIME DESTINATION . )
endif(NOT WIN32)

INCLUDE(CMakeForceCompiler)
CMAKE_FORCE_CXX_COMPILER(${R_CXX} R_CXX)

