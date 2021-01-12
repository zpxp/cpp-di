cmake_minimum_required(VERSION 3.13)

if(POLICY CMP0076)
   cmake_policy(SET CMP0076 NEW)
endif()

set (CMAKE_BUILD_TYPE Release)
unset (SRC_DIR CACHE)
get_filename_component(ABSOLUTE_DIR "${PROJECT_SOURCE_DIR}"  REALPATH BASE_DIR )
set (SRC_DIR "${ABSOLUTE_DIR}/src" CACHE STRING "SRC variable")
set (CMAKE_CXX_STANDARD 17)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(${SOURCE_DIR})

project(cpp-di)

add_subdirectory (src) 

set_target_properties(cpp_di PROPERTIES
    VERSION 1
    SOVERSION 1
	 PUBLIC_HEADER src/di.h)
	 
install(DIRECTORY ${SRC_DIR}/ # source directory
	 DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cpp_di
	 FILES_MATCHING # install only matched files
	 PATTERN "*.h" # select header files
)

include(GNUInstallDirs)
install(TARGETS cpp_di
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	 PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cpp_di)
	 
configure_file(cpp_di.pc.in cpp_di.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/cpp_di.pc DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)