cmake_minimum_required(VERSION 3.16)

project(SandboxDll LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


file(GLOB_RECURSE SANDBOXDLL_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" 
)

add_library(SandboxDll SHARED "${SANDBOXDLL_SOURCES}")

target_include_directories(SandboxDll PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_link_libraries(SandboxDll PRIVATE ProjectZvend)
