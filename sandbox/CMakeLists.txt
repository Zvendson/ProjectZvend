cmake_minimum_required(VERSION 3.16)

project(Sandbox LANGUAGES CXX)

add_executable(Sandbox)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


file(GLOB_RECURSE SANDBOX_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" 
)

target_sources(Sandbox PRIVATE "${SANDBOX_SOURCES}")
target_include_directories(Sandbox PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_link_libraries(Sandbox PRIVATE ProjectZvend)
