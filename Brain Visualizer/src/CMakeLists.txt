file(GLOB_RECURSE SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/data/*.cpp"
)

add_executable(BrainViz ${SOURCES})

target_link_libraries(BrainViz PRIVATE
        simdjson
        fmt
        tsl::robin_map
)

target_include_directories(BrainViz PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)