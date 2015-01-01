include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(qt)

install( FILES smoke.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include )
install( FILES qt_smoke.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include/smoke )
