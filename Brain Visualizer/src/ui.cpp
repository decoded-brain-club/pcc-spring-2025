#include <imgui-SFML.h>
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <iostream>
#include <unordered_set>

#include <ui/electrode_visualization.hpp>
#include <data/interface.hpp>
#include <data/json_file_source.hpp>
#include <analysis/batch_analyzer.hpp>
#include <electrode/electrode_set.hpp>

#include <ui/frequency_band_selector.hpp>
#include <ui/detail/electrode_state_manager.hpp>

#include <implot.h>
#include <logging/logger.hpp>

#include <fonts/roboto-regular.hpp>

#include "../cmake-build-relwithdebinfo/_deps/robin-map-src/include/tsl/robin_set.h"

using namespace brainviz;

void embraceTheDarkness()
{
    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
    colors[ImGuiCol_Border] = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
    colors[ImGuiCol_Button] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
    colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
    colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TableBorderStrong] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TableBorderLight] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowPadding = ImVec2(8.00f, 8.00f);
    style.FramePadding = ImVec2(5.00f, 2.00f);
    style.CellPadding = ImVec2(6.00f, 6.00f);
    style.ItemSpacing = ImVec2(6.00f, 6.00f);
    style.ItemInnerSpacing = ImVec2(6.00f, 6.00f);
    style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
    style.IndentSpacing = 25;
    style.ScrollbarSize = 15;
    style.GrabMinSize = 10;
    style.WindowBorderSize = 1;
    style.ChildBorderSize = 1;
    style.PopupBorderSize = 1;
    style.FrameBorderSize = 1;
    style.TabBorderSize = 1;
    style.WindowRounding = 7;
    style.ChildRounding = 4;
    style.FrameRounding = 3;
    style.PopupRounding = 4;
    style.ScrollbarRounding = 9;
    style.GrabRounding = 3;
    style.LogSliderDeadzone = 4;
    style.TabRounding = 4;
}

// TODO: move this to a separate file
class EEGVisualizer
{
public:
    explicit EEGVisualizer(const ElectrodeStateManager& stateManager)
        : m_stateManager(stateManager)
    {
        initialize_implot();
    }

    ~EEGVisualizer()
    {
        // CLEAN UP IS HANDLED BY WHOEVER SETS UP THE IMGUI/IMPLOT CONTEXT
    }

    void render(sf::RenderWindow& window, const std::vector<sf::Vector2f>& electrodePoints) const
    {
        const sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        const sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

        // check if any ImGui window is being hovered/clicked, this inhibits electrode interaction so you dont click on an electrode and a window at the same time
        const bool mouseOverWindow = ImGui::IsAnyItemHovered() || ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;

        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && !m_wasLeftMousePressed)
        {
            // check each for a click
            if (!mouseOverWindow)
            {
                for (size_t i = 0; i < electrodePoints.size(); ++i)
                {
                    // 30*30 hitbox
                    sf::FloatRect boundingBox(
                        {
                            electrodePoints[i].x - 15.0f, // left
                            electrodePoints[i].y - 15.0f
                        }, // top
                        {
                            30.0f, // width
                            30.0f
                        } // height
                    );

                    if (boundingBox.contains(mousePosF))
                    {
                        const int electrodeId = static_cast<int>(i);

                        if (m_openElectrodeWindows.count(electrodeId) > 0)
                        {
                            m_openElectrodeWindows.erase(electrodeId);
                        }
                        else
                        {
                            m_openElectrodeWindows.insert(electrodeId);

                            // initial window position
                            m_windowPositions[electrodeId] = ImVec2(
                                electrodePoints[i].x + 30.0f,
                                electrodePoints[i].y - 30.0f
                            );
                        }
                        break; // one electrode click at a time
                    }
                }
            }
        }

        // update mouse state
        m_wasLeftMousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

        // normal rendering
        if (m_stateManager.is_single_band_mode())
        {
            render_single_band(window, electrodePoints);
        }
        else
        {
            render_all_bands(window, electrodePoints);
        }

        //if (m_showDebugBoxes)
        //{
        //    for (const auto& point : electrodePoints)
        //    {
        //        sf::RectangleShape rect(sf::Vector2f(30.0f, 30.0f));
        //        rect.setPosition({point.x - 15.0f, point.y - 15.0f});
        //        rect.setFillColor(sf::Color::Transparent);
        //        rect.setOutlineColor(sf::Color(255, 255, 255, 64));
        //        rect.setOutlineThickness(1.0f);
        //        window.draw(rect);
        //    }
        //}

        // high electrodes with open windows
        for (const auto& electrodeId : m_openElectrodeWindows)
        {
            if (electrodeId >= 0 && electrodeId < static_cast<int>(electrodePoints.size()))
            {
                const sf::Vector2f& pos = electrodePoints[electrodeId];

                // draw a highlight circle around the electrode
                sf::CircleShape highlight(18.0f);
                highlight.setOrigin({18.0f, 18.0f});
                highlight.setPosition(pos);
                highlight.setFillColor(sf::Color::Transparent);
                highlight.setOutlineColor(sf::Color(255, 255, 255, 180));
                highlight.setOutlineThickness(2.0f);
                window.draw(highlight);
            }
        }

        // render all imgui windows for all open electrodes
        std::vector<int> windowsToClose;

        for (const auto& electrodeId : m_openElectrodeWindows)
        {
            bool windowOpen = true;
            if (show_electrode_window(electrodeId, electrodePoints, windowOpen) && !windowOpen)
            {
                // q for closing
                windowsToClose.push_back(electrodeId);
            }
        }

        for (int id : windowsToClose)
        {
            m_openElectrodeWindows.erase(id);
        }
    }


private:
    const ElectrodeStateManager& m_stateManager;

    // states
    mutable bool m_wasLeftMousePressed = false;
    mutable bool m_implotAvailable = false;

    // TODO: make these use vector
    // open electrode windows
    mutable tsl::robin_set<int> m_openElectrodeWindows;

    // window positions
    mutable tsl::robin_map<int, ImVec2> m_windowPositions;

    // selected band for time series (per electrode)
    mutable tsl::robin_map<int, int> m_selectedBandForTimeSeries;

    mutable tsl::robin_map<int, bool> m_showRawWaveform;
    mutable tsl::robin_map<int, size_t> m_rawDataWindowSizes;

    // implot availability sanity check
    void initialize_implot() const
    {
        // check if ImPlot is available, avoids the context not created thing assertion failure
        ImGuiContext* ctx = ImGui::GetCurrentContext();
        if (ctx != nullptr)
        {
            // try to access the context
            try
            {
                void* implot_ctx = ImPlot::GetCurrentContext();
                m_implotAvailable = (implot_ctx != nullptr);

                if (!m_implotAvailable)
                {
                    g_logger.warn("ImPlot context not available. Plots will not be shown.");
                }
            }
            catch (...)
            {
                m_implotAvailable = false;
                g_logger.warn("Exception when checking ImPlot availability. Plots will not be shown.");
            }
        }
        else
        {
            m_implotAvailable = false;
            g_logger.warn("ImGui context not available. Plots will not be shown.");
        }
    }

    static sf::Color get_band_color(const data::FrequencyBand band)
    {
        switch (band)
        {
            case data::FrequencyBand::Delta:
                return {170, 223, 237};
            case data::FrequencyBand::Theta:
                return {207, 230, 184};
            case data::FrequencyBand::Alpha:
                return {251, 247, 186};
            case data::FrequencyBand::Beta:
                return {255, 214, 166};
            case data::FrequencyBand::Gamma:
                return {248, 172, 174};
            default:
                return {255, 255, 255};
        }
    }

    static ImVec4 get_imgui_color(const sf::Color& color, float alpha = 1.0f)
    {
        return ImVec4(
            static_cast<float>(color.r) / 255.0f,
            static_cast<float>(color.g) / 255.0f,
            static_cast<float>(color.b) / 255.0f,
            alpha
        );
    }

    void render_single_band(sf::RenderWindow& window, const std::vector<sf::Vector2f>& electrodePoints) const
    {
        const brainviz::data::FrequencyBand selectedBand = m_stateManager.get_selected_band();
        const int bandIndex = static_cast<int>(selectedBand);
        const auto& visualizationData = m_stateManager.get_visualization_data();

        const sf::Color color = get_band_color(selectedBand);

        static constexpr float baseRadius = 30.0f;

        for (const auto& [id, data] : visualizationData)
        {
            if (id < 0 || id >= static_cast<int>(electrodePoints.size()))
                continue;

            const sf::Vector2f& position = electrodePoints[id];

            float radius = baseRadius * data.radii[bandIndex];

            sf::Color electrodeColor = color;
            electrodeColor.a = static_cast<uint8_t>(255 * data.alphas[bandIndex]);

            sf::CircleShape circle(radius);
            circle.setOrigin({radius, radius});
            circle.setPosition(position);
            circle.setFillColor(electrodeColor);
            window.draw(circle);
        }
    }

    void render_all_bands(sf::RenderWindow& window, const std::vector<sf::Vector2f>& electrodePoints) const
    {
        const brainviz::data::FrequencyBand selectedBand = m_stateManager.get_selected_band();
        const auto& visualizationData = m_stateManager.get_visualization_data();

        for (const auto& [id, data] : visualizationData)
        {
            if (id < 0 || id >= static_cast<int>(electrodePoints.size()))
                continue;

            const sf::Vector2f& position = electrodePoints[id];

            for (int i = 0; i < 5; ++i)
            {
                const auto band = static_cast<brainviz::data::FrequencyBand>(i);

                const float baseRadius = 6.0f + i * 3.0f;
                float radius = baseRadius * data.radii[i];

                sf::Color color = get_band_color(band);
                color.a = static_cast<uint8_t>(255 * data.alphas[i]);

                if (band == selectedBand)
                {
                    color.a = 255;
                }

                sf::CircleShape circle(radius);
                circle.setOrigin({radius, radius});
                circle.setPosition(position);
                circle.setFillColor(color);
                window.draw(circle);
            }
        }
    }

    // i mean realistically this shouldnt even be here
    std::array<double, 5> get_electrode_band_amplitudes(int electrodeId, const std::string& channelName) const
    {
        std::array<double, 5> result = {0.0, 0.0, 0.0, 0.0, 0.0};

        try
        {
            const auto& analyzer = m_stateManager.get_analyzer();
            const size_t frameIndex = m_stateManager.get_frame_index();

            for (int i = 0; i < 5; i++)
            {
                const auto band = static_cast<brainviz::data::FrequencyBand>(i);
                const auto& bandData = analyzer.get_band_amplitude(band, channelName);

                if (frameIndex < bandData.size())
                {
                    result[i] = bandData[frameIndex];
                }
            }
        }
        catch (const std::exception& e)
        {
            g_logger.warn("Failed to get band amplitudes for electrode {}: {}", electrodeId, e.what());

            // fallback to visualization data
            // TODO: should be reflected in the UI clearly
            const auto& visualizationData = m_stateManager.get_visualization_data();

            if (const auto it = visualizationData.find(electrodeId); it != visualizationData.end())
            {
                for (int i = 0; i < 5; i++)
                {
                    // Convert normalized radius back to amplitude (rough approximation)
                    result[i] = it->second.radii[i];
                }
            }
        }

        return result;
    }

    // electrode window, returns true if the window was rendered
    bool show_electrode_window(int electrodeId,
                               const std::vector<sf::Vector2f>& electrodePoints,
                               bool& windowOpen) const
    {
        if (electrodeId < 0 || electrodeId >= static_cast<int>(electrodePoints.size()))
            return false;

        const sf::Vector2f& position = electrodePoints[electrodeId];

        std::string channelName = "Unknown";
        try
        {
            const auto& electrodeSet = m_stateManager.get_electrode_set();
            channelName = electrodeSet.find(electrodeId).value().name();
        }
        catch (const std::exception& e) // really shouldnt happen honestly
        {
            g_logger.warn("Could not get channel name for electrode {}: {}", electrodeId, e.what());
        }

        // make a unique window id with the electrode ID and channel name
        std::string windowTitle = fmt::format("Electrode {} ({})###Electrode{}",
                                              electrodeId, channelName, electrodeId);

        // set the window position if we have it stored
        if (auto posIt = m_windowPositions.find(electrodeId); posIt != m_windowPositions.end())
        {
            ImGui::SetNextWindowPos(posIt->second, ImGuiCond_FirstUseEver);
        }
        else
        {
            ImGui::SetNextWindowPos(ImVec2(position.x + 30, position.y - 30), ImGuiCond_FirstUseEver);
        }

        ImGui::SetNextWindowSize(ImVec2(500, 600), ImGuiCond_FirstUseEver);

        // init selected band for time series if not already set
        if (m_selectedBandForTimeSeries.find(electrodeId) == m_selectedBandForTimeSeries.end())
        {
            // default to the current one
            m_selectedBandForTimeSeries[electrodeId] = static_cast<int>(m_stateManager.get_selected_band());
        }

        // init raw waveform display setting if not already set
        if (m_showRawWaveform.find(electrodeId) == m_showRawWaveform.end())
        {
            // default to showing raw waveform
            m_showRawWaveform[electrodeId] = true;
        }

        // init raw data window size if not already set
        if (m_rawDataWindowSizes.find(electrodeId) == m_rawDataWindowSizes.end())
        {
            // TODO: default window size (in samples), customizability
            m_rawDataWindowSizes[electrodeId] = 1000;
        }

        bool windowVisible = ImGui::Begin(windowTitle.c_str(), &windowOpen);

        // only render if its not collapsed
        if (windowVisible)
        {
            try
            {
                const auto& analyzer = m_stateManager.get_analyzer();

                ImGui::Text("Electrode ID: %d", electrodeId);
                ImGui::Text("Channel: %s", channelName.c_str());
                ImGui::Text("Position: (%.1f, %.1f)", position.x, position.y);

                size_t frameIndex = m_stateManager.get_frame_index();
                size_t maxFrameIndex = m_stateManager.get_max_frame_index();
                size_t windowSize = analyzer.get_window_size();
                size_t hopSize = analyzer.get_hop_size();
                size_t timeIndex = m_stateManager.compute_time_index(frameIndex);

                ImGui::Text("Frame: %zu / %zu", frameIndex, maxFrameIndex);
                ImGui::Text("Time Index: %zu", timeIndex);

                double samplingRate = analyzer.get_sampling_rate();
                double freqResolution = analyzer.get_frequency_resolution();
                ImGui::Text("Sampling Rate: %.1f Hz", samplingRate);
                ImGui::Text("Window Size: %zu samples (%.3f sec)", windowSize, windowSize / samplingRate);
                ImGui::Text("Frequency Resolution: %.3f Hz", freqResolution);

                ImGui::Separator();

                std::array<double, 5> bandAmplitudes = get_electrode_band_amplitudes(electrodeId, channelName);
                const char* bandNames[] = {"Delta", "Theta", "Alpha", "Beta", "Gamma"};

                ImGui::BeginTable("BandTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg);
                ImGui::TableSetupColumn("Band");
                ImGui::TableSetupColumn("Range (Hz)");
                ImGui::TableSetupColumn("Amplitude (μV)");
                ImGui::TableHeadersRow();

                // Delta: 0.5-4 Hz
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextColored(get_imgui_color(get_band_color(data::FrequencyBand::Delta)), "Delta");
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("0.5-4 Hz");
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%.2f", bandAmplitudes[0]);

                // theta: 4-8 Hz
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextColored(get_imgui_color(get_band_color(data::FrequencyBand::Theta)), "Theta");
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("4-8 Hz");
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%.2f", bandAmplitudes[1]);

                // alpha: 8-13 Hz
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextColored(get_imgui_color(get_band_color(data::FrequencyBand::Alpha)), "Alpha");
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("8-13 Hz");
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%.2f", bandAmplitudes[2]);

                // beta: 13-30 Hz
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextColored(get_imgui_color(get_band_color(data::FrequencyBand::Beta)), "Beta");
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("13-30 Hz");
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%.2f", bandAmplitudes[3]);

                // gama: 30-100 Hz
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextColored(get_imgui_color(get_band_color(data::FrequencyBand::Gamma)), "Gamma");
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("30-100 Hz");
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%.2f", bandAmplitudes[4]);

                ImGui::EndTable();

                ImGui::Separator();

                int& selectedBand = m_selectedBandForTimeSeries[electrodeId];
                ImGui::Text("Select band for time series:");
                for (int i = 0; i < 5; i++)
                {
                    sf::Color sfColor = get_band_color(static_cast<brainviz::data::FrequencyBand>(i));
                    ImVec4 imColor = get_imgui_color(sfColor);

                    ImGui::SameLine();
                    ImGui::PushID(i);
                    ImGui::PushStyleColor(ImGuiCol_Text, imColor);
                    if (ImGui::RadioButton(bandNames[i], &selectedBand, i))
                    {
                        // selection changed
                    }
                    ImGui::PopStyleColor();
                    ImGui::PopID();
                }

                ImGui::Separator();

                // time series plot for the selected frequency band
                if (m_implotAvailable)
                {
                    if (ImPlot::BeginPlot("Band Amplitude Time Series", ImVec2(-1, 200)))
                    {
                        ImPlot::SetupAxes("Time (s)", "Amplitude (μV)",
                                          ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);

                        std::vector<double> times;
                        std::vector<double> amplitudes;

                        // the batch data
                        bool dataAvailable = false;
                        try
                        {
                            const auto& band = static_cast<brainviz::data::FrequencyBand>(selectedBand);
                            const auto& bandData = analyzer.get_band_amplitude(band, channelName);

                            // show the last N frames to give context
                            const size_t numFramesToShow = std::min<size_t>(100, bandData.size());
                            const size_t startFrame = (frameIndex >= numFramesToShow)
                                                          ? (frameIndex - numFramesToShow + 1)
                                                          : 0;

                            // fill the vectors with actual data
                            for (size_t i = startFrame; i <= frameIndex && i < bandData.size(); i++)
                            {
                                double timeInSeconds = (static_cast<double>(i) * hopSize) / samplingRate;
                                times.push_back(timeInSeconds);
                                amplitudes.push_back(bandData[i]);
                            }

                            dataAvailable = !times.empty();
                        }
                        catch (const std::exception& e)
                        {
                            g_logger.warn("Failed to get band data: {}", e.what());
                            dataAvailable = false;
                        }

                        // If we can't get actual time series, show just the current value
                        // TODO: reflect this in the UI
                        if (!dataAvailable)
                        {
                            double timeInSeconds = static_cast<double>(timeIndex) / samplingRate;
                            times.push_back(timeInSeconds);
                            amplitudes.push_back(bandAmplitudes[selectedBand]);
                        }

                        // plot the time series
                        if (!times.empty())
                        {
                            sf::Color sfColor =
                                    get_band_color(static_cast<brainviz::data::FrequencyBand>(selectedBand));
                            ImVec4 imColor = get_imgui_color(sfColor);

                            ImPlot::SetNextLineStyle(imColor, 2.0f);
                            ImPlot::PlotLine(bandNames[selectedBand],
                                             times.data(), amplitudes.data(),
                                             static_cast<int>(times.size()));

                            if (dataAvailable && !amplitudes.empty())
                            {
                                double currentTime = times.back();
                                ImPlot::DragLineX(1, &currentTime, ImVec4(1, 1, 1, 0.7f), 1.0f,
                                                  ImPlotDragToolFlags_NoInputs);

                                ImPlot::Annotation(currentTime, amplitudes.back(), ImVec4(1, 1, 1, 1), ImVec2(0, -5),
                                                   true, "%.2f μV", amplitudes.back());
                            }
                        }

                        ImPlot::EndPlot();
                    }
                }
                else
                {
                    // fallback when ImPlot is not available
                    ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.3f, 1.0f),
                                       "ImPlot is not available. Visualization disabled.");

                    ImGui::Text("Current %s amplitude: %.2f μV",
                                bandNames[selectedBand],
                                bandAmplitudes[selectedBand]);
                }

                bool& showRawWaveform = m_showRawWaveform[electrodeId];
                ImGui::Checkbox("Show Raw EEG Waveform", &showRawWaveform);

                if (showRawWaveform)
                {
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(200);
                    size_t& rawWindowSize = m_rawDataWindowSizes[electrodeId];
                    int windowSizeInt = static_cast<int>(rawWindowSize);
                    if (ImGui::SliderInt("Window Size (samples)", &windowSizeInt, 100, 5000))
                    {
                        rawWindowSize = static_cast<size_t>(windowSizeInt);
                    }
                }

                // raw data plot
                if (showRawWaveform && m_implotAvailable)
                {
                    try
                    {
                        const auto& eegData = analyzer.get_eeg_data();
                        const auto& rawChannelData = eegData.get_channel(channelName);

                        // calc the window range to display
                        size_t rawWindowSize = m_rawDataWindowSizes[electrodeId];
                        size_t halfWindow = rawWindowSize / 2;
                        size_t startIdx = (timeIndex > halfWindow) ? timeIndex - halfWindow : 0;
                        size_t endIdx = std::min(startIdx + rawWindowSize, rawChannelData.size());

                        // if we hit the end, adjust the start
                        if (endIdx == rawChannelData.size() && endIdx > rawWindowSize)
                        {
                            startIdx = endIdx - rawWindowSize;
                        }

                        double startTimeSec = startIdx / samplingRate;
                        double endTimeSec = endIdx / samplingRate;
                        // std::string plotTitle = fmt::format("Raw EEG ({:.2} - {:.2} sec)", startTimeSec, endTimeSec);
                        // TODO: come up with a nice way of displaying the time range, if we update the srting in beginplot everything falls apart

                        if (ImPlot::BeginPlot("Raw EEG", ImVec2(-1, 250)))
                        {
                            ImPlot::SetupAxes("Time (s)", "Amplitude (μV)",
                                              ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);

                            std::vector<double> timeValues;
                            std::vector<double> amplitudeValues;

                            timeValues.reserve(endIdx - startIdx);
                            amplitudeValues.reserve(endIdx - startIdx);

                            for (size_t i = startIdx; i < endIdx; ++i)
                            {
                                timeValues.push_back(i / samplingRate);
                                amplitudeValues.push_back(rawChannelData[i]);
                            }

                            ImPlot::SetNextLineStyle(ImVec4(1, 1, 1, 1), 1.0f);
                            ImPlot::PlotLine("Raw EEG",
                                             timeValues.data(),
                                             amplitudeValues.data(),
                                             static_cast<int>(timeValues.size()));

                            if (windowSize > 0)
                            {
                                // start and end
                                size_t winStart = (timeIndex > windowSize / 2) ? timeIndex - windowSize / 2 : 0;
                                size_t winEnd = winStart + windowSize;

                                // mark IF we are within view
                                if ((winStart >= startIdx && winStart < endIdx) ||
                                    (winEnd > startIdx && winEnd <= endIdx) ||
                                    (winStart <= startIdx && winEnd >= endIdx))
                                {
                                    double winStartTime = std::max(winStart, startIdx) / samplingRate;
                                    double winEndTime = std::min(winEnd, endIdx) / samplingRate;

                                    // calc y range
                                    double minVal = *std::ranges::min_element(amplitudeValues);
                                    double maxVal = *std::ranges::max_element(amplitudeValues);
                                    double padding = (maxVal - minVal) * 0.1;

                                    // the shaded area for current window
                                    ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);

                                    ImVec4 windowColor = get_imgui_color(
                                        get_band_color(static_cast<brainviz::data::FrequencyBand>(selectedBand)));
                                    ImPlot::SetNextFillStyle(windowColor);

                                    double xs[2] = {winStartTime, winEndTime};
                                    double ys1[2] = {minVal, minVal}; // bottom line
                                    double ys2[2] = {maxVal, maxVal}; // top line

                                    // PlotShaded with xs, ys1, ys2, count
                                    ImPlot::PlotShaded("##Window", xs, ys1, ys2, 2);

                                    ImPlot::PopStyleVar();

                                    ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 2.0f);
                                    ImPlot::DragLineX(1, &winStartTime, ImVec4(1, 1, 1, 0.5f), 1.0f,
                                                      ImPlotDragToolFlags_NoInputs);
                                    ImPlot::DragLineX(2, &winEndTime, ImVec4(1, 1, 1, 0.5f), 1.0f,
                                                      ImPlotDragToolFlags_NoInputs);
                                    ImPlot::PopStyleVar();

                                    ImPlot::Annotation(
                                        (winStartTime + winEndTime) / 2, maxVal + padding,
                                        ImVec4(1, 1, 1, 1), ImVec2(0, -5), true,
                                        "Analysis Window (%zu samples)", windowSize);
                                }
                            }

                            double currentTimeSec = timeIndex / samplingRate;
                            if (currentTimeSec >= startTimeSec && currentTimeSec <= endTimeSec)
                            {
                                ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 3.0f);
                                ImPlot::DragLineX(0, &currentTimeSec, ImVec4(1, 0, 0, 1), 1.0f,
                                                  ImPlotDragToolFlags_NoInputs);
                                ImPlot::PopStyleVar();

                                // closest amplitude to the current time
                                size_t closestIdx = static_cast<size_t>(std::round((currentTimeSec - startTimeSec) *
                                                        samplingRate)) + startIdx;
                                if (closestIdx < endIdx)
                                {
                                    double amplitude = rawChannelData[closestIdx];
                                    ImPlot::Annotation(currentTimeSec, amplitude, ImVec4(1, 0, 0, 1), ImVec2(0, 10),
                                                       true, "Current (%.2f μV)", amplitude);
                                }
                            }

                            ImPlot::EndPlot();
                        }
                    }
                    catch (const std::exception& e)
                    {
                        ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f),
                                           "Error accessing raw EEG data: %s", e.what());
                    }
                }
            }
            catch (const std::exception& e)
            {
                ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f),
                                   "Error displaying electrode data: %s", e.what());
            }
        }
        ImGui::End();




        return true;
    }
};

int main()
{
    // Get desktop resolution and set aspect ratio
    const sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    const unsigned int screenWidth = desktopMode.size.x;
    const unsigned int screenHeight = desktopMode.size.y;
    const float aspectRatio = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);

    // Create a full-screen window
    sf::RenderWindow window(desktopMode, "BrainViz EEG Visualizer",
                            sf::Style::Default,
                            sf::State::Windowed,
                            sf::ContextSettings(0, 0, 8));

    window.setFramerateLimit(144);
    const sf::Vector2u windowSize = window.getSize();

    ImGui::SFML::Init(window);


    ImGuiIO& io = ImGui::GetIO();
    ImFont* font = io.Fonts->AddFontFromMemoryTTF((void *) fonts::roboto_regular.data(), fonts::roboto_regular_size,
                                                  14.0f);
    io.Fonts->Build();
    io.FontDefault = font;

    ImGui::SFML::UpdateFontTexture();

    embraceTheDarkness();

    ImPlot::CreateContext();

    // Head variables
    const double HEAD_RADIUS = (windowSize.y / 2.0) * 0.7;
    const double CIRCLE_RADIUS = HEAD_RADIUS * 1.2;

    // Create the 64-channel electrode set
    brainviz::electrode::ElectrodeSet electrodeSet(brainviz::electrode::SystemType::System64);

    // Head (outer circle)
    sf::CircleShape head(static_cast<float>(CIRCLE_RADIUS));
    head.setFillColor({0, 0, 0, 0});
    head.setOutlineThickness(3);
    head.setOutlineColor({255, 255, 255});
    head.setPosition({
        static_cast<float>(windowSize.x / 2.0 - head.getRadius()),
        static_cast<float>(windowSize.y / 2.0 - head.getRadius())
    });

    std::cout << "Loading EEG data from JSON file..." << std::endl;

    auto dataSource = std::make_unique<brainviz::data::JSONFileSource>("data.json");
    if (!dataSource->open())
    {
        std::cerr << "Error: Failed to open data.json file" << std::endl;
        return 1;
    }

    auto eegData = dataSource->load_data();
    if (!eegData)
    {
        std::cerr << "Error: Failed to load data from data.json" << std::endl;
        return 1;
    }

    std::cout << "JSON data loaded successfully" << std::endl;

    std::cout << "Processing EEG data..." << std::endl;
    brainviz::analysis::BatchAnalyzer analyzer(*eegData, 128, 75.0);
    analyzer.process_all_channels();
    std::cout << "Data processing complete" << std::endl;

    FrequencyBandSelector bandSelector;

    ElectrodeStateManager stateManager(electrodeSet, analyzer);

    EEGVisualizer visualizer(stateManager);

    std::vector<sf::Vector2f> points;

    brainviz::visualization::draw_electrodes(
        window, electrodeSet, points, HEAD_RADIUS, CIRCLE_RADIUS, windowSize
    );

    sf::Clock deltaClock;

    while (window.isOpen())
    {
        // Process events
        while (const std::optional<sf::Event> event = window.pollEvent())
        {
            ImGui::SFML::ProcessEvent(window, *event);

            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            if (event->is<sf::Event::Resized>())
            {
                brainviz::visualization::enforce_aspect_ratio(window, aspectRatio);
            }
        }

        const sf::Time dt = deltaClock.restart();
        ImGui::SFML::Update(window, dt);

        const float deltaTimeSeconds = dt.asSeconds();

        stateManager.update(deltaTimeSeconds, bandSelector.get_animation_speed());

        bandSelector.set_frame_info(stateManager.get_frame_index(), stateManager.get_max_frame_index());

        bandSelector.render();

        window.clear({20, 20, 30});

        window.draw(head);

        brainviz::visualization::draw_electrodes(
            window, electrodeSet, points, HEAD_RADIUS, CIRCLE_RADIUS, windowSize
        );
        brainviz::visualization::draw_frame(window, brainviz::electrode::SystemType::System64, points);

        visualizer.render(window, points);

        ImGui::SFML::Render(window);

        window.display();
    }

    ImPlot::DestroyContext();

    ImGui::SFML::Shutdown();

    return 0;
}
