cmake_minimum_required(VERSION 3.30.5)
project(BrainViz VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(cmake/CPM.cmake)

CPMAddPackage(
        NAME simdjson
        VERSION 3.12.2
        GITHUB_REPOSITORY simdjson/simdjson
)

CPMAddPackage("gh:fmtlib/fmt#11.1.4")
CPMAddPackage("gh:Tessil/robin-map@1.3.0")

# find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(src)

install(TARGETS BrainViz
        RUNTIME DESTINATION bin
)