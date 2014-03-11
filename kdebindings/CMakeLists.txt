cmake_minimum_required(VERSION 2.8.9)

project(kdebindings)

if(POLICY CMP0020)
  cmake_policy(SET CMP0020 OLD)
endif()

find_package(Qt5Core, 5.2.0)

macro(qt5_optional_use_modules _target)
  foreach(_module ${ARGN})
    find_package(Qt5${_module} 5.2.0 QUIET)
    if(Qt5${_module}_FOUND)
      qt5_use_modules(${_target} ${_module})
      message(STATUS "Found Qt module: ${_module}")
    endif()
  endforeach()
endmacro()

set(CMAKE_CXX_FLAGS "-fPIC")

add_subdirectory(generator)
add_subdirectory(smoke)
