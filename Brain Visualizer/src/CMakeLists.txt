set(SOURCES
        # "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/ui.cpp"
        # "${CMAKE_CURRENT_SOURCE_DIR}/imgui_basic.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/data/json_file_source.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/data/streaming_source.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/analysis/batch_analyzer.cpp"

        "${CMAKE_CURRENT_SOURCE_DIR}/ui/electrode_renderer.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/ui/electrode_frame.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/ui/detail/electrode_state_manager.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/ui/frequency_band_selector.cpp"

        "${CMAKE_CURRENT_SOURCE_DIR}/utils/visualization_utils.cpp"


)

add_executable(BrainViz ${SOURCES})

target_precompile_headers(BrainViz PRIVATE
        <vector>
        <string>
        <memory>
        <unordered_map>
        <algorithm>

        <fmt/format.h>
        <simdjson.h>
        <tsl/robin_map.h>
        <kfr/all.hpp>
        <SFML/Graphics.hpp>
        <imgui.h>
)

set(CMAKE_PCH_INSTANTIATE_TEMPLATES ON)

target_link_libraries(BrainViz PRIVATE
        simdjson
        fmt
        tsl::robin_map
        kfr_dft
        SFML::Graphics
        ImGui-SFML::ImGui-SFML
)

target_include_directories(BrainViz PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/res
)