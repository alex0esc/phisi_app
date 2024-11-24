cmake_minimum_required(VERSION 3.2)
project(phsSimTools VERSION 1.0.0 LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/release)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib/debug)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib/release)

include_directories(${CMAKE_SOURCE_DIR}/include)
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(SOURCES 
  ${SOURCE_DIR}/main.cpp
) 

add_executable(${PROJECT_NAME} ${SOURCES})

set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall")   # Debug flags (for debugging)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")      # Release flags (for optimization)
