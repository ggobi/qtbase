cmake_minimum_required(VERSION 2.6)

project(kdebindings)

add_subdirectory(generator)

#set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules )

#include (MacroOptionalFindPackage)

FIND_PACKAGE(Perl REQUIRED)
FIND_PACKAGE(Qt4 REQUIRED)

add_definitions (${QT_DEFINITIONS}  -DHAVE_CONFIG_H=1)
include_directories (${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${QT_INCLUDES} )

if( UNIX )
   # Generator library is used during compilation. If some environment not
   # have previous kdebindings installed, will fail, then we need add local path
   set( ENV{LD_LIBRARY_PATH} ${CMAKE_CURRENT_BINARY_DIR}/generator/bin )
endif( UNIX )

#FIND_PACKAGE (OpenGL REQUIRED)

add_subdirectory(smoke)
