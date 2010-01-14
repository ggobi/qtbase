OPTION(ENABLE_SMOKE "build Smoke" ON)

set(SMOKE_ENABLED "no")
if(ENABLE_SMOKE)
    add_subdirectory(qt)
    set(SMOKE_ENABLED "yes - Qt")
endif(ENABLE_SMOKE)

message(STATUS "Build Smoke... " ${SMOKE_ENABLED})

install( FILES  smoke.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include )
