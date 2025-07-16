#project
cmake_minimum_required(VERSION 3.20)
project(phisi_app VERSION 1.0.0 LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

#C++ Standart
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/release)

#compile command for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#create necessary directories
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/assets)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/config)

#source files
file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB IMGUI_CPPS "${CMAKE_CURRENT_SOURCE_DIR}/imgui/*.cpp")
list(APPEND SOURCES ${IMGUI_CPPS})
set(SHADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
set(SHADERS 
    ${SHADER_DIR}/force.comp
    ${SHADER_DIR}/velocity.comp
    ${SHADER_DIR}/divergence.comp
    ${SHADER_DIR}/color.comp
)


#glfw
set(GLFW_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui/glfw-3.4) # Set this to point to an up-to-date GLFW repo
option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
option(GLFW_INSTALL "Generate installation target" OFF)
option(GLFW_DOCUMENT_INTERNALS "Include internals in documentation" OFF)
add_subdirectory(${GLFW_DIR} binary_dir EXCLUDE_FROM_ALL)

#submodules
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/logger)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/phisi)

#compiltaion
add_executable(${PROJECT_NAME} ${SOURCES})

#include directories and libraries
target_link_libraries(${PROJECT_NAME} PRIVATE logger)
target_link_libraries(${PROJECT_NAME} PRIVATE phisi)
target_link_libraries(${PROJECT_NAME} PRIVATE winmm)
find_package(Vulkan REQUIRED)
#find_library(VULKAN_LIBRARY
#NAMES vulkan vulkan-1)
#set(LIBRARIES "glfw;${VULKAN_LIBRARY}")
set(VULKAN_LIBRARIES "glfw;Vulkan::Vulkan")
target_link_libraries(${PROJECT_NAME} PRIVATE ${VULKAN_LIBRARIES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/imgui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

#compile shader
set(COMPILED_SHADERS)
add_custom_target(ShaderCode ALL DEPENDS ${SHADERS})

foreach(SHADER ${SHADERS})    
get_filename_component(FILE_NAME ${SHADER} NAME_WE)
set(OUTPUT_PATH ${SHADER_DIR}/${FILE_NAME}.spv) 

add_custom_command(
    TARGET ShaderCode
    PRE_BUILD
    COMMAND glslangValidator -V ${SHADER} -o ${OUTPUT_PATH} -Os)
    
list(APPEND COMPILED_SHADERS ${OUTPUT_PATH})
endforeach()

add_custom_target(Shaders ALL DEPENDS ${COMPILED_SHADERS})
add_dependencies(${PROJECT_NAME} Shaders)

#build flags
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -g -flto")

#pass macro to c code 
target_compile_definitions(${PROJECT_NAME} PRIVATE PROJECT_NAME="${PROJECT_NAME}")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE BUILD_DEBUG)
endif()
