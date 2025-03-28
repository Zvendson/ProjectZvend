cmake_minimum_required(VERSION 3.16)

project(ProjectZvend VERSION 0.1.0 LANGUAGES CXX)

option(ENABLE_SANDBOX "Enables sandbox example project." OFF)

add_subdirectory(deps/base64)
add_subdirectory(deps/imgui)
add_subdirectory(deps/json)
add_subdirectory(deps/minhook)
add_subdirectory(deps/spdlog)


set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_target_properties(base64         PROPERTIES FOLDER “Libs”)
set_target_properties(imgui          PROPERTIES FOLDER “Libs”)
set_target_properties(nlohmann_json  PROPERTIES FOLDER “Libs”)
set_target_properties(minhook        PROPERTIES FOLDER “Libs”)
set_target_properties(spdlog         PROPERTIES FOLDER “Libs”)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(ProjectZvend)

file(GLOB_RECURSE SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
)

file(GLOB_RECURSE HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/ProjectZvend/*.hpp"
)

target_sources(ProjectZvend
    PUBLIC
        "${HEADERS}"
    PRIVATE
        "${SOURCES}"
)

target_include_directories(ProjectZvend
    PUBLIC
        "include/"
    PRIVATE
        "src/"
)

target_precompile_headers(ProjectZvend
    PRIVATE
        "<algorithm>"
        "<array>"
        "<cassert>"
        "<chrono>"
        "<cstdint>"
        "<cstddef>"
        "<cstring>"
        "<filesystem>"
        "<fstream>"
        "<functional>"
        "<iostream>"
        "<map>"
        "<memory>"
        "<mutex>"
        "<sstream>"
        "<string>"
        "<thread>"
        "<tuple>"
        "<type_traits>"
        "<unordered_map>"
        "<utility>"
        "<vector>"

        "<nlohmann/json.hpp>"
        "<minhook.h>"
        "<spdlog/spdlog.h>"
        "<imgui.h>"
        "<base64.hpp>"
)

target_link_libraries(ProjectZvend
    PUBLIC
        spdlog::spdlog
        imgui
        nlohmann_json::nlohmann_json
        minhook
        base64
        Dbghelp
)

if(ENABLE_SANDBOX)
    add_subdirectory(sandboxDll)
    add_subdirectory(sandbox)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Sandbox)
endif()
