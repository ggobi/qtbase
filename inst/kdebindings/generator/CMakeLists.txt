project(smokegenerator)

find_package(Qt4 REQUIRED)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

include_directories(${QT_INCLUDES} parser)

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

add_executable(generator ${generator_SRC})
target_link_libraries(generator ${QT_QTCORE_LIBRARY} ${QT_QTXML_LIBRARY} cppparser)
set_target_properties(generator PROPERTIES COMPILE_DEFINITIONS __GENERATOR_BUILDING ENABLE_EXPORTS TRUE)

add_subdirectory(generators)
