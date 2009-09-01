OPTION(ENABLE_SMOKE "build Smoke" ON)

set(SMOKE_ENABLED "no")
if(ENABLE_SMOKE)
    add_subdirectory(qt)
    set(SMOKE_ENABLED "yes - Qt")
endif(ENABLE_SMOKE)

# for qtguess.pl
if (APPLE)
set(qt_test_threshold 15)
else (APPLE)
set(qt_test_threshold 10)
endif (APPLE)

message(STATUS "Build Smoke... " ${SMOKE_ENABLED})


########### install files ###############

# KDE specific
#install( FILES  smoke.h DESTINATION ${INCLUDE_INSTALL_DIR} )

# Qt only
#install( FILES  smoke.h DESTINATION ${INSTALL_DIR}/include )

install( FILES  smoke.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include )
