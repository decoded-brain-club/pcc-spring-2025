#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include <logging/logger.hpp>
#include <iostream>
#include <string>
#include <cmath>
#include <optional>

#include <imgui/seekbar.hpp>

void embraceTheDarkness()
{
  ImVec4* colors = ImGui::GetStyle().Colors;
  colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
  colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
  colors[ImGuiCol_WindowBg]               = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
  colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  colors[ImGuiCol_PopupBg]                = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
  colors[ImGuiCol_Border]                 = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
  colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
  colors[ImGuiCol_FrameBg]                = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
  colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
  colors[ImGuiCol_FrameBgActive]          = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
  colors[ImGuiCol_TitleBg]                = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_TitleBgActive]          = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
  colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
  colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
  colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
  colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
  colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
  colors[ImGuiCol_CheckMark]              = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
  colors[ImGuiCol_SliderGrab]             = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
  colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
  colors[ImGuiCol_Button]                 = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
  colors[ImGuiCol_ButtonHovered]          = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
  colors[ImGuiCol_ButtonActive]           = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
  colors[ImGuiCol_Header]                 = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
  colors[ImGuiCol_HeaderHovered]          = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
  colors[ImGuiCol_HeaderActive]           = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
  colors[ImGuiCol_Separator]              = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
  colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
  colors[ImGuiCol_SeparatorActive]        = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
  colors[ImGuiCol_ResizeGrip]             = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
  colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
  colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
  colors[ImGuiCol_Tab]                    = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
  colors[ImGuiCol_TabHovered]             = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
  colors[ImGuiCol_TabActive]              = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
  colors[ImGuiCol_TabUnfocused]           = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
  colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
  colors[ImGuiCol_PlotLines]              = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_PlotHistogram]          = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
  colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
  colors[ImGuiCol_TableBorderLight]       = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
  colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
  colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
  colors[ImGuiCol_DragDropTarget]         = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
  colors[ImGuiCol_NavHighlight]           = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
  colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
  colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

  ImGuiStyle& style = ImGui::GetStyle();
  style.WindowPadding                     = ImVec2(8.00f, 8.00f);
  style.FramePadding                      = ImVec2(5.00f, 2.00f);
  style.CellPadding                       = ImVec2(6.00f, 6.00f);
  style.ItemSpacing                       = ImVec2(6.00f, 6.00f);
  style.ItemInnerSpacing                  = ImVec2(6.00f, 6.00f);
  style.TouchExtraPadding                 = ImVec2(0.00f, 0.00f);
  style.IndentSpacing                     = 25;
  style.ScrollbarSize                     = 15;
  style.GrabMinSize                       = 10;
  style.WindowBorderSize                  = 1;
  style.ChildBorderSize                   = 1;
  style.PopupBorderSize                   = 1;
  style.FrameBorderSize                   = 1;
  style.TabBorderSize                     = 1;
  style.WindowRounding                    = 7;
  style.ChildRounding                     = 4;
  style.FrameRounding                     = 3;
  style.PopupRounding                     = 4;
  style.ScrollbarRounding                 = 9;
  style.GrabRounding                      = 3;
  style.LogSliderDeadzone                 = 4;
  style.TabRounding                       = 4;
}

int main()
{
    // Initialize window
    sf::RenderWindow window(sf::VideoMode({1920u, 1080u}), "ImGui Seek Bar Demo");
    window.setFramerateLimit(60);

    // Initialize ImGui
    if (!ImGui::SFML::Init(window, true)) {
        std::cerr << "ImGui SFML initialization failed\n";
        return -1;
    }

    embraceTheDarkness();

    // Create seek bar widget
    ImGuiSeekBarWidget seekBar;

    // Configure the seek bar
    seekBar.SetDuration(300.0f); // 5 minutes duration
    seekBar.SetProgress(0.0f);   // Start at the beginning

    // Set colors for better appearance
    seekBar.SetTrackColor(ImGuiSeekBarWidget::CreateColor(0.2f, 0.2f, 0.2f));
    seekBar.SetActiveTrackColor(ImGuiSeekBarWidget::CreateColor(0.0f, 0.6f, 1.0f));
    seekBar.SetThumbColor(ImGuiSeekBarWidget::CreateColor(0.9f, 0.9f, 0.9f));

    // Add some chapter markers
    seekBar.AddChapterMarker(30.0f, "Introduction");
    seekBar.AddChapterMarker(75.0f, "Chapter 1");
    seekBar.AddChapterMarker(150.0f, "Chapter 2");
    seekBar.AddChapterMarker(225.0f, "Conclusion");

    // Set up a callback to handle progress jumps
    seekBar.SetProgressJumpCallback([](float progress) {
        std::cout << "Jumped to position: " << progress * 100.0f << "%\n";
    });

    // Configure keyboard shortcuts
    ImGuiSeekBarWidget::KeyConfig keyConfig;
    keyConfig.togglePlay = ImGuiKey_Space;
    keyConfig.seekLeft = ImGuiKey_LeftArrow;
    keyConfig.seekRight = ImGuiKey_RightArrow;
    keyConfig.reset = ImGuiKey_R;
    keyConfig.seekAmount = 0.02f; // 2% jumps
    seekBar.SetKeyConfig(keyConfig);

    // Animation state
    sf::Clock clock;
    sf::Clock playbackClock;

    // Main loop
    while (window.isOpen()) {
        // Handle events
        while (const std::optional event = window.pollEvent())
        {
            ImGui::SFML::ProcessEvent(window, *event);

            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }



        // Update ImGui
        sf::Time deltaTime = clock.restart();
        ImGui::SFML::Update(window, deltaTime);

        // Update playback if playing
        if (seekBar.IsPlaying()) {
            sf::Time playbackDelta = playbackClock.restart();
            float progressDelta = playbackDelta.asSeconds() * seekBar.GetPlaybackSpeed() / seekBar.GetDuration();
            float newProgress = seekBar.GetProgress() + progressDelta;

            // Loop back to beginning if we reach the end
            if (newProgress >= 1.0f) {
                newProgress = 0.0f;
            }

            seekBar.SetProgress(newProgress);
        } else {
            // Reset the playback clock when paused to avoid jumps
            playbackClock.restart();
        }

        // Create a demo control panel
        ImGui::Begin("Seek Bar Demo Controls");

        // Display current time and progress
        float currentTime = seekBar.GetCurrentTime();
        int minutes = static_cast<int>(currentTime) / 60;
        float seconds = currentTime - (minutes * 60.0f);
        ImGui::Text("Current Time: %02d:%05.2f", minutes, seconds);
        ImGui::Text("Progress: %.1f%%", seekBar.GetProgress() * 100.0f);
        ImGui::Text("Playback Speed: %.2fx", seekBar.GetPlaybackSpeed());
        ImGui::Text("Status: %s", seekBar.IsPlaying() ? "Playing" : "Paused");

        ImGui::Separator();

        // Manual controls for the demo
        if (ImGui::Button("Play/Pause")) {
            seekBar.SetPlaying(!seekBar.IsPlaying());
        }

        ImGui::SameLine();

        if (ImGui::Button("Reset")) {
            seekBar.SetProgress(0.0f);
        }

        // Manual progress control
        float progress = seekBar.GetProgress();
        if (ImGui::SliderFloat("Set Position", &progress, 0.0f, 1.0f, "%.2f")) {
            seekBar.SetProgress(progress);
        }

        ImGui::Separator();
        ImGui::Text("Keyboard Controls:");
        ImGui::Text("Space: Play/Pause");
        ImGui::Text("Left/Right Arrows: Seek");
        ImGui::Text("R: Reset to beginning");

        ImGui::End();

        // Render the seek bar widget
        seekBar.Render();

        // Render everything
        window.clear(sf::Color(30, 30, 30));
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}