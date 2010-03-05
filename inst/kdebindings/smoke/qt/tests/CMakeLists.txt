cmake_minimum_required(VERSION 2.6)
project(target_test)
enable_language(CXX)

# # set(test NONE)
# message(STATUS "Target test: ${test}")
# add_definitions(-DTEST_${test})
add_executable( target_test test.cpp )
target_link_libraries( target_test ${QT_LIBRARIES} )
