cmake_minimum_required(VERSION 2.6)

project(kdebindings)

add_subdirectory(generator)

FIND_PACKAGE(Qt4 REQUIRED 4.0.0 COMPONENTS QtCore QtGui QtXml QtXmlPatterns
  QtNetwork QtOpenGL QtSql QtSvg QtDBus QtScript QtWebkit
  QtTest QtUiTools phonon REQUIRED)

add_subdirectory(smoke)

if( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
  set_target_properties(smokeqt PROPERTIES COMPILE_FLAGS "-fPIC")
endif( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )

