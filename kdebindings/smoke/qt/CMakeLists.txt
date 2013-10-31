include_directories (${CMAKE_CURRENT_SOURCE_DIR})

set(smokeqt_LIB_SRCS ${CMAKE_CURRENT_BINARY_DIR}/smokedata.cpp  
                ${CMAKE_CURRENT_BINARY_DIR}/x_1.cpp 
                ${CMAKE_CURRENT_BINARY_DIR}/x_2.cpp
                ${CMAKE_CURRENT_BINARY_DIR}/x_3.cpp
                ${CMAKE_CURRENT_BINARY_DIR}/x_4.cpp
                ${CMAKE_CURRENT_BINARY_DIR}/x_5.cpp
                ${CMAKE_CURRENT_BINARY_DIR}/x_6.cpp
                ${CMAKE_CURRENT_BINARY_DIR}/x_7.cpp
                ${CMAKE_CURRENT_BINARY_DIR}/x_8.cpp
                ${CMAKE_CURRENT_BINARY_DIR}/x_9.cpp
                ${CMAKE_CURRENT_BINARY_DIR}/x_10.cpp
                ${CMAKE_CURRENT_BINARY_DIR}/x_11.cpp
                ${CMAKE_CURRENT_BINARY_DIR}/x_12.cpp
                ${CMAKE_CURRENT_BINARY_DIR}/x_13.cpp
                ${CMAKE_CURRENT_BINARY_DIR}/x_14.cpp
                ${CMAKE_CURRENT_BINARY_DIR}/x_15.cpp
                ${CMAKE_CURRENT_BINARY_DIR}/x_16.cpp
                ${CMAKE_CURRENT_BINARY_DIR}/x_17.cpp
                ${CMAKE_CURRENT_BINARY_DIR}/x_18.cpp
                ${CMAKE_CURRENT_BINARY_DIR}/x_19.cpp
                ${CMAKE_CURRENT_BINARY_DIR}/x_20.cpp
)

IF(CMAKE_CXX_FLAGS MATCHES "-fvisibility")
    ADD_DEFINITIONS(-DGCC_VISIBILITY)
ENDIF(CMAKE_CXX_FLAGS MATCHES "-fvisibility")

add_definitions(-DBASE_SMOKE_BUILDING)

# kde4_add_library(smokeqt SHARED ${smokeqt_LIB_SRCS})
add_library(smokeqt STATIC ${smokeqt_LIB_SRCS})

set_target_properties(smokeqt PROPERTIES VERSION 3.0.0 SOVERSION 3 )

if( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
  set_target_properties(smokeqt PROPERTIES COMPILE_FLAGS "-fPIC")
endif( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )

INCLUDE(CMakeForceCompiler)
CMAKE_FORCE_CXX_COMPILER(${R_CXX} R_CXX)

qt5_use_modules(smokeqt Widgets)

set(QT_MODULES Multimedia MultimediaWidgets Network Qml Quick Sql Test
               Webkit WebKitWidgets DBus Svg XmlPatterns
               PrintSupport Help UiTools SerialPort Sensors
               Bluetooth NFC Positioning)

qt5_optional_use_modules(smokeqt ${QT_MODULES})

foreach(_module ${QT_MODULES})
  LIST(APPEND QT_INCLUDE_DIRS ${Qt5${_module}_INCLUDE_DIRS})
endforeach()
LIST(REMOVE_DUPLICATES QT_INCLUDE_DIRS)
foreach(_dir ${QT_INCLUDE_DIRS})
  set(SMOKE_CONFIG_INCLUDE_DIR_ELEMENTS
      "${SMOKE_CONFIG_INCLUDE_DIR_ELEMENTS}\t\t<dir>${_dir}</dir>\n")
endforeach()

configure_file(config.xml.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.xml @ONLY)

if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/../qtdefines)
foreach(qt_module ${QT_MODULES})
  if(Qt5${qt_module}_FOUND)
    string(TOUPPER ${qt_module} QT_MODULE)
    file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/../qtdefines "QT_${QT_MODULE}_LIB\n")
  endif()
endforeach()
endif()

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/smokedata.cpp 
    ${CMAKE_CURRENT_BINARY_DIR}/x_1.cpp 
    ${CMAKE_CURRENT_BINARY_DIR}/x_2.cpp 
    ${CMAKE_CURRENT_BINARY_DIR}/x_3.cpp 
    ${CMAKE_CURRENT_BINARY_DIR}/x_4.cpp 
    ${CMAKE_CURRENT_BINARY_DIR}/x_5.cpp 
    ${CMAKE_CURRENT_BINARY_DIR}/x_6.cpp 
    ${CMAKE_CURRENT_BINARY_DIR}/x_7.cpp 
    ${CMAKE_CURRENT_BINARY_DIR}/x_8.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/x_9.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/x_10.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/x_11.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/x_12.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/x_13.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/x_14.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/x_15.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/x_16.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/x_17.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/x_18.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/x_19.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/x_20.cpp

    COMMAND smokegen
    ARGS -config "${CMAKE_CURRENT_BINARY_DIR}/config.xml"
         -smokeconfig "${CMAKE_CURRENT_SOURCE_DIR}/smokeconfig.xml"
         -d "${CMAKE_CURRENT_BINARY_DIR}/../qtdefines"
         -- "${CMAKE_CURRENT_SOURCE_DIR}/qt_includes.h"
    DEPENDS smokegen generator_smoke ${CMAKE_CURRENT_BINARY_DIR}/config.xml
            smokeconfig.xml qt_includes.h
            ${CMAKE_CURRENT_BINARY_DIR}/../qtdefines
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

macro_add_file_dependencies( ${CMAKE_CURRENT_BINARY_DIR}/smokedata.cpp ${CMAKE_CURRENT_BINARY_DIR}/x_1.cpp )

# Don't use the KDE ${LIB_INSTALL_DIR} macro here as this file is
# for Qt only installs
# install(TARGETS smokeqt DESTINATION lib${LIB_SUFFIX} )

install( FILES ${CMAKE_CURRENT_BINARY_DIR}/../qtdefines
  ${CMAKE_CURRENT_BINARY_DIR}/config.xml DESTINATION share/kdebindings )
