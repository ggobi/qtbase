OPTION(ENABLE_SMOKE "build Smoke" ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(SMOKE_ENABLED "no")
if(ENABLE_SMOKE)
    add_subdirectory(qt)
    set(SMOKE_ENABLED "yes - Qt")
endif(ENABLE_SMOKE)

message(STATUS "Build Smoke... " ${SMOKE_ENABLED})

install( FILES smoke.h DESTINATION include )
install(FILES qt_smoke.h DESTINATION include/smoke)
