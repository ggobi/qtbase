cmake_minimum_required(VERSION 2.8.8)

project(qtbase)

if(POLICY CMP0020)
  cmake_policy(SET CMP0020 OLD)
endif()

find_package(Qt5Core 5.2.0)

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

qt5_optional_use_modules(qtbase Multimedia MultimediaWidgets Network Qml Quick
                         Sql Test WebKit WebKitWidgets DBus Svg XmlPatterns
                         PrintSupport Help UiTools SerialPort Sensors
                         Bluetooth Positioning Nfc WebSockets)

## QtUiTools not linked correctly on Windows (_personality_v0 missing symbol)
if(WIN32)
set(QT_USE_QTUITOOLS FALSE)
endif(WIN32)

##set_target_properties(qtbase PROPERTIES COMPILE_FLAGS "-ggdb")
set_target_properties(qtbase PROPERTIES COMPILE_FLAGS "-std=c++11")

## Force optional Qt5 modules to come after smokeqt, so all needed symbols are linked
set(LIB_LOC "")
if(Qt5Multimedia_FOUND)
  get_target_property(QtMultimedia_location Qt5::Multimedia LOCATION)
  set(LIB_LOC ${LIB_LOC} ${QtMultimedia_location})
endif(Qt5Multimedia_FOUND)
if(Qt5MultimediaWidgets_FOUND)
  get_target_property(QtMultimediaWidgets_location Qt5::MultimediaWidgets LOCATION)
  set(LIB_LOC ${LIB_LOC} ${QtMultimediaWidgets_location})
endif(Qt5MultimediaWidgets_FOUND)
if(Qt5Network_FOUND)
  get_target_property(QtNetwork_location Qt5::Network LOCATION)
  set(LIB_LOC ${LIB_LOC} ${QtNetwork_location})
endif(Qt5Network_FOUND)
if(Qt5Qml_FOUND)
  get_target_property(QtQml_location Qt5::Qml LOCATION)
  set(LIB_LOC ${LIB_LOC} ${QtQml_location})
endif(Qt5Qml_FOUND)
if(Qt5Quick_FOUND)
  get_target_property(QtQuick_location Qt5::Quick LOCATION)
  set(LIB_LOC ${LIB_LOC} ${QtQuick_location})
endif(Qt5Quick_FOUND)
if(Qt5Sql_FOUND)
  get_target_property(QtSql_location Qt5::Sql LOCATION)
  set(LIB_LOC ${LIB_LOC} ${QtSql_location})
endif(Qt5Sql_FOUND)
if(Qt5Test_FOUND)
  get_target_property(QtTest_location Qt5::Test LOCATION)
  set(LIB_LOC ${LIB_LOC} ${QtTest_location})
endif(Qt5Test_FOUND)
if(Qt5WebKit_FOUND)
  get_target_property(QtWebKit_location Qt5::WebKit LOCATION)
  set(LIB_LOC ${LIB_LOC} ${QtWebKit_location})
endif(Qt5WebKit_FOUND)
if(Qt5WebKitWidgets_FOUND)
  get_target_property(QtWebKitWidgets_location Qt5::WebKitWidgets LOCATION)
  set(LIB_LOC ${LIB_LOC} ${QtWebKitWidgets_location})
endif(Qt5WebKitWidgets_FOUND)
if(Qt5DBus_FOUND)
  get_target_property(QtDBus_location Qt5::DBus LOCATION)
  set(LIB_LOC ${LIB_LOC} ${QtDBus_location})
endif(Qt5DBus_FOUND)
if(Qt5Svg_FOUND)
  get_target_property(QtSvg_location Qt5::Svg LOCATION)
  set(LIB_LOC ${LIB_LOC} ${QtSvg_location})
endif(Qt5Svg_FOUND)
if(Qt5XmlPatterns_FOUND)
  get_target_property(QtXmlPatterns_location Qt5::XmlPatterns LOCATION)
  set(LIB_LOC ${LIB_LOC} ${QtXmlPatterns_location})
endif(Qt5XmlPatterns_FOUND)
if(Qt5PrintSupport_FOUND)
  get_target_property(QtPrintSupport_location Qt5::PrintSupport LOCATION)
  set(LIB_LOC ${LIB_LOC} ${QtPrintSupport_location})
endif(Qt5PrintSupport_FOUND)
if(Qt5Help_FOUND)
  get_target_property(QtHelp_location Qt5::Help LOCATION)
  set(LIB_LOC ${LIB_LOC} ${QtHelp_location})
endif(Qt5Help_FOUND)
if(Qt5UiTools_FOUND)
  get_target_property(QtUiTools_location Qt5::UiTools LOCATION)
  set(LIB_LOC ${LIB_LOC} ${QtUiTools_location})
endif(Qt5UiTools_FOUND)
if(Qt5SerialPort_FOUND)
  get_target_property(QtSerialPort_location Qt5::SerialPort LOCATION)
  set(LIB_LOC ${LIB_LOC} ${QtSerialPort_location})
endif(Qt5SerialPort_FOUND)
if(Qt5Sensors_FOUND)
  get_target_property(QtSensors_location Qt5::Sensors LOCATION)
  set(LIB_LOC ${LIB_LOC} ${QtSensors_location})
endif(Qt5Sensors_FOUND)
if(Qt5Bluetooth_FOUND)
  get_target_property(QtBluetooth_location Qt5::Bluetooth LOCATION)
  set(LIB_LOC ${LIB_LOC} ${QtBluetooth_location})
endif(Qt5Bluetooth_FOUND)
if(Qt5Positioning_FOUND)
  get_target_property(QtPositioning_location Qt5::Positioning LOCATION)
  set(LIB_LOC ${LIB_LOC} ${QtPositioning_location})
endif(Qt5Positioning_FOUND)
if(Qt5Nfc_FOUND)
  get_target_property(QtNfc_location Qt5::Nfc LOCATION)
  set(LIB_LOC ${LIB_LOC} ${QtNfc_location})
endif(Qt5Nfc_FOUND)
target_link_libraries(qtbase smokeqt ${R_LIBRARIES} ${LIB_LOC})

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

