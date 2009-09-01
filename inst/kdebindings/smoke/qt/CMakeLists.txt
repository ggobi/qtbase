
include_directories( ${QT_INCLUDES} ${CMAKE_SOURCE_DIR}/smoke )


IF(NOT PERL_FOUND)
    FIND_PACKAGE(Perl REQUIRED)
ENDIF(NOT PERL_FOUND)

# for qtguess.pl
if (APPLE)
set(qt_test_threshold 15)
else (APPLE)
set(qt_test_threshold 10)
endif (APPLE)

include(QtGuess.txt)

########### next target ###############

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

                  COMMAND ${PERL_EXECUTABLE} ARGS ${CMAKE_CURRENT_BINARY_DIR}/generate.pl
                  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/generate.pl

                  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

macro_add_file_dependencies( ${CMAKE_CURRENT_BINARY_DIR}/smokedata.cpp ${CMAKE_CURRENT_BINARY_DIR}/x_1.cpp )

include_directories (${CMAKE_SOURCE_DIR}/smoke ${CMAKE_SOURCE_DIR}/smoke/qt  )

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

# kde4_add_library(smokeqt SHARED ${smokeqt_LIB_SRCS})
add_library(smokeqt SHARED ${smokeqt_LIB_SRCS})

SET(QT_USE_QTNETWORK ${QT_QTNETWORK_FOUND})
SET(QT_USE_QTOPENGL ${QT_QTOPENGL_FOUND})
SET(QT_USE_QTSQL ${QT_QTSQL_FOUND})
SET(QT_USE_QTXML ${QT_QTXML_FOUND})
SET(QT_USE_QTSVG ${QT_QTSVG_FOUND})
SET(QT_USE_QTUITOOLS ${QT_QTUITOOLS_FOUND})
SET(QT_USE_QT3SUPPORT ${QT_QT3SUPPORT_FOUND})
#SET(QT_USE_QTDBUS ${QT_QTDBUS_FOUND)
# The above line doesn't work with cmake 2.4.6, but should be fixed in later versions

INCLUDE(${QT_USE_FILE})
# At this point, QT_LIBRARIES will contain a list of the found Qt Libs
# and QT_INCLUDES contains the directories of the found includes

FOREACH(filename ${QT_INCLUDES})
    set(all_includes "-I${filename} ${all_includes}")
ENDFOREACH(filename)

FOREACH(incname ${QT_INCLUDES})
    SET(qt_incs "'${incname}', ${qt_incs}")
ENDFOREACH(incname)

FOREACH(libname ${QT_LIBRARIES})
    set(all_libs "${libname} ${all_libs}")
ENDFOREACH(libname)

target_link_libraries(smokeqt ${QT_QTCORE_LIBRARY}
                              ${QT_QTGUI_LIBRARY}
                              ${QT_QTNETWORK_LIBRARY}
                              ${QT_QTOPENGL_LIBRARY}
                              ${QT_QTDBUS_LIBRARY}
                              ${QT_QTSQL_LIBRARY}
                              ${QT_QTSVG_LIBRARY}
                              ${QT_QTXML_LIBRARY} )

set_target_properties(smokeqt PROPERTIES VERSION 2.0.0 SOVERSION 2 )

# Don't use the KDE ${LIB_INSTALL_DIR} macro here as this file is
# for Qt only installs
install(TARGETS smokeqt DESTINATION ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX} )
install(FILES ${CMAKE_SOURCE_DIR}/smoke/qt_smoke.h DESTINATION include/smoke)

# configure_file(qtguess.pl.cmake ${CMAKE_CURRENT_BINARY_DIR}/qtguess.pl @ONLY)
configure_file(generate.pl.cmake ${CMAKE_CURRENT_BINARY_DIR}/generate.pl @ONLY )
