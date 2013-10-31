include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../..
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../../smoke)

set(generator_smoke_SRC
    generator_smoke.cpp
    writeClasses.cpp
    writeSmokeDataFile.cpp
    helpers.cpp)

add_library(generator_smoke MODULE ${generator_smoke_SRC})
target_link_libraries(generator_smoke smokebase ${QT_QTXML_LIBRARY} smokegen)
set_target_properties(generator_smoke PROPERTIES PREFIX "")

qt5_use_modules(generator_smoke Core Xml)

install(TARGETS generator_smoke LIBRARY DESTINATION ${LIB_INSTALL_DIR}/smokegen/)

INCLUDE(CMakeForceCompiler)
CMAKE_FORCE_CXX_COMPILER(${R_CXX} R_CXX)
