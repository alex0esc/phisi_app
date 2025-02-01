#project
cmake_minimum_required(VERSION 3.20)
project(logger VERSION 1.0.0 LANGUAGES CXX)

#C++ Standart
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib/debug)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib/release)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib/debug)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib/release)

#options
option(LOG_ERROR "Enable logging errors" ON)
option(LOG_WARN "Enable logging warnings" ON)
option(LOG_INFO "Enable logging info" ON)
option(LOG_TRACE "Enable logging traces" OFF)

#compilation
file(GLOB CPPS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB HPPS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")
add_library(${PROJECT_NAME} STATIC ${CPPS} ${HPPS})

#include
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

#definitions
if(LOG_ERROR)
  add_compile_definitions(${PROJECT_NAME} PUBLIC _OPTION_LOG_ERROR)
endif()

if(LOG_WARN)
  target_compile_definitions(${PROJECT_NAME} PUBLIC _OPTION_LOG_WARN)
endif()

if(LOG_INFO)
  target_compile_definitions(${PROJECT_NAME} PUBLIC _OPTION_LOG_INFO)
endif()

if(LOG_TRACE)
  target_compile_definitions(${PROJECT_NAME} PUBLIC _OPTION_LOG_TRACE)
endif()

#preproccessed header
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/logger.hpp ${CMAKE_CURRENT_SOURCE_DIR}/include/logger_pre.hpp)