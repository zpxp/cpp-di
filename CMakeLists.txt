cmake_minimum_required(VERSION 3.13)

if(POLICY CMP0076)
   cmake_policy(SET CMP0076 NEW)
endif()

set (CMAKE_BUILD_TYPE Debug)
unset (SRC_DIR CACHE)
get_filename_component(ABSOLUTE_DIR "${PROJECT_SOURCE_DIR}"  REALPATH BASE_DIR )
set (SRC_DIR "${ABSOLUTE_DIR}/src" CACHE STRING "SRC variable")
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-delete-non-abstract-non-virtual-dtor -g -std=c++17")
set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(${SOURCE_DIR})

project(cpp-di)

add_subdirectory (src) 


target_sources(xc_compiler
   PRIVATE
		src/Container.h
		src/Container.cxx
		src/Module.h
		src/Module.cxx
		src/Registration.h
   PUBLIC
		src/Injector.h
)

