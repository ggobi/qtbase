cmake_minimum_required(VERSION 2.6)

project(kdebindings)

add_subdirectory(generator)

FIND_PACKAGE(Qt4 REQUIRED)

add_subdirectory(smoke)