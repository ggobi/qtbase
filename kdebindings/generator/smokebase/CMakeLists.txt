IF(CMAKE_CXX_FLAGS MATCHES "-fvisibility")
    ADD_DEFINITIONS(-DGCC_VISIBILITY)
ENDIF(CMAKE_CXX_FLAGS MATCHES "-fvisibility")

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/..
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../smoke)

add_library(smokebase SHARED smokebase.cpp)
target_link_libraries(smokebase)
set_target_properties(smokebase PROPERTIES 
                                VERSION ${SMOKE_VERSION}
                                SOVERSION 3)

include(MacroWriteBasicCMakeVersionFile.cmake)
macro_write_basic_cmake_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/SmokeVersionConfig.cmake
    ${SMOKE_VERSION_MAJOR}
    ${SMOKE_VERSION_MINOR}
    ${SMOKE_VERSION_PATCH}
)

install(TARGETS smokebase LIBRARY DESTINATION ${LIB_INSTALL_DIR}
    ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
    RUNTIME DESTINATION bin)
if (WIN32)
	# Realign the stack, for compatibility with an older ABI.
	if(CMAKE_COMPILER_IS_GNUCXX)
		set_target_properties(smokebase PROPERTIES COMPILE_FLAGS -mstackrealign)
	endif()

    # Get rid of the "lib" prefix on archives/DLLs in Windows.
    set_target_properties(smokebase PROPERTIES PREFIX "" IMPORT_PREFIX "")
endif (WIN32)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/SmokeVersionConfig.cmake DESTINATION ${CMAKE_INSTALL_PREFIX}/share/smoke/cmake)
