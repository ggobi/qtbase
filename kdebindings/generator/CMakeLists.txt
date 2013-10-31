project(smokegenerator)

set(SMOKE_VERSION_MAJOR 3)
set(SMOKE_VERSION_MINOR 0)
set(SMOKE_VERSION_PATCH 0)
set(SMOKE_VERSION ${SMOKE_VERSION_MAJOR}.${SMOKE_VERSION_MINOR}.${SMOKE_VERSION_PATCH})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

if (NOT LIB_INSTALL_DIR)
    set (LIB_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX})
endif (NOT LIB_INSTALL_DIR)

include_directories(parser)

add_subdirectory(parser)

set(generator_SRC
    main.cpp
    generatorenvironment.cpp
    generatorvisitor.cpp
    generatorpreprocessor.cpp
    options.cpp
    type_compiler.cpp
    name_compiler.cpp
    type.cpp
)

# force RPATH so that the binary is usable from within the build tree
set (CMAKE_SKIP_BUILD_RPATH FALSE)
set (CMAKE_SKIP_RPATH FALSE)

add_executable(smokegen ${generator_SRC})
target_link_libraries(smokegen ${QT_QTCORE_LIBRARY} ${QT_QTXML_LIBRARY} cppparser)
set_target_properties(smokegen PROPERTIES COMPILE_DEFINITIONS __GENERATOR_BUILDING ENABLE_EXPORTS TRUE)

qt5_use_modules(smokegen Core Xml)

install(TARGETS smokegen DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
install(FILES generatorenvironment.h  generator_export.h  generatorpreprocessor.h  generatorvisitor.h  name_compiler.h  options.h  type_compiler.h  type.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include/smokegen)
install( FILES smokegen_string.h DESTINATION ${CMAKE_INSTALL_PREFIX}/share/smokegen RENAME string.h )

add_subdirectory(smokebase)
add_subdirectory(generators)

INCLUDE(CMakeForceCompiler)
CMAKE_FORCE_CXX_COMPILER(${R_CXX} R_CXX)
