project(smokegenerator)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

if (NOT LIB_INSTALL_DIR)
    set (LIB_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/lib)
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

add_executable(smokegen ${generator_SRC})
target_link_libraries(smokegen ${QT_QTCORE_LIBRARY} ${QT_QTXML_LIBRARY} cppparser)
set_target_properties(smokegen PROPERTIES COMPILE_DEFINITIONS __GENERATOR_BUILDING ENABLE_EXPORTS TRUE)

install(TARGETS smokegen DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

add_subdirectory(generators)
