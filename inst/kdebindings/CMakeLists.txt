cmake_minimum_required(VERSION 2.6)

project(kdebindings)

add_subdirectory(generator)

FIND_PACKAGE(Qt4 REQUIRED)

add_subdirectory(smoke)

if( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
  set_target_properties(smokeqt PROPERTIES COMPILE_FLAGS "-fPIC")
endif( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )

