cmake_minimum_required(VERSION 2.6)

project(kdebindings)

#set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules )

#include (MacroOptionalFindPackage)

FIND_PACKAGE(Perl REQUIRED)
FIND_PACKAGE(Qt4 REQUIRED)

add_definitions (${QT_DEFINITIONS}  -DHAVE_CONFIG_H=1)
include_directories (${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${QT_INCLUDES} )

#FIND_PACKAGE (OpenGL REQUIRED)

add_subdirectory(smoke)
