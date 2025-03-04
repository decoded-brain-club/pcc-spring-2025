cmake_minimum_required(VERSION 3.30.5)
project(BrainViz VERSION 0.1.0 LANGUAGES CXX)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    message(STATUS "Found ccache: ${CCACHE_PROGRAM}")
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
else()
    message(STATUS "ccache not found")
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(cmake/CPM.cmake)
include(FetchContent)

CPMAddPackage(
        NAME simdjson
        VERSION 3.12.2
        GITHUB_REPOSITORY simdjson/simdjson
)

CPMAddPackage("gh:fmtlib/fmt#11.1.4")
CPMAddPackage("gh:Tessil/robin-map@1.3.0")

set(KFR_ENABLE_DFT ON)
set(ENABLE_TESTS OFF)
set("KFR_ENABLE_CAPI OFF")
set("KFR_ENABLE_MULTIARCH OFF")
CPMAddPackage("gh:kfrlib/kfr#6.1.0")

FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.0
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM)
FetchContent_MakeAvailable(SFML)

FetchContent_Declare(ImGui
        GIT_REPOSITORY https://github.com/ocornut/imgui
        GIT_TAG v1.91.1
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM)
FetchContent_MakeAvailable(ImGui)
FetchContent_GetProperties(ImGui SOURCE_DIR IMGUI_DIR)

set(IMGUI_SFML_FIND_SFML OFF)
FetchContent_Declare(ImGui-SFML
        GIT_REPOSITORY https://github.com/SFML/imgui-sfml
        GIT_TAG v3.0
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM)
FetchContent_MakeAvailable(ImGui-SFML)

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(src)

install(TARGETS BrainViz
        RUNTIME DESTINATION bin
)