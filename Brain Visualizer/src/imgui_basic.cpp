#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include <logging/logger.hpp>
#include <functional>
#include <string>
#include <algorithm>
#include <cmath>

#include <cstdio>
#include <vector>
#include <imgui_internal.h>


class ImGuiSeekBarWidget
{
public:
    // Callback type for progress jumps
    using ProgressJumpCallback = std::function<void(float)>;

    // Chapter/marker data structure
    struct ChapterMarker
    {
        float time{};
        std::string name;
        ImVec4 color = ImVec4(144.0f, 144.0f, 144.0f, 144.0f);
    };

    // Add key configuration for customizable keybindings
    struct KeyConfig
    {
        ImGuiKey togglePlay = ImGuiKey_Space;
        ImGuiKey seekLeft = ImGuiKey_LeftArrow;
        ImGuiKey seekRight = ImGuiKey_RightArrow;
        ImGuiKey reset = ImGuiKey_R;
    };

    ImGuiSeekBarWidget()
        : m_currentProgress(0.0f)
          , m_duration(100.0f)
          , m_isPlaying(false)
          , m_playbackSpeed(1.0f)
          , m_progressChanged(false)
          , m_trackColor(IM_COL32(100, 100, 100, 255))
          , m_activeTrackColor(IM_COL32(50, 150, 200, 255))
          , m_thumbColor(IM_COL32(200, 200, 200, 255))
          , m_chapterMarkerColor(IM_COL32(255, 255, 0, 255))
          , m_textColor(IM_COL32(255, 255, 255, 255))
          , m_addingMarker(false)
          , m_newMarkerName("")
          , m_handleKeyboardInput(true)
    {
    }

    // Main render function for the seek bar widget
    void Render()
    {
        // Handle keyboard input if enabled
        if (m_handleKeyboardInput)
        {
            HandleKeyboardInput();
        }

        // Set up an invisible window with the requested flags
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar |
                                 ImGuiWindowFlags_NoResize |
                                 ImGuiWindowFlags_NoMove |
                                 ImGuiWindowFlags_NoScrollbar |
                                 ImGuiWindowFlags_NoCollapse |
                                 ImGuiWindowFlags_NoBackground;

        // Always use fullscreen dimensions
        ImVec2 viewportSize = ImGui::GetMainViewport()->Size;
        ImGui::SetNextWindowSize(ImVec2(viewportSize.x * 0.8f, viewportSize.y * 0.2f));
        ImGui::SetNextWindowPos(ImVec2(viewportSize.x * 0.1f, viewportSize.y * 0.75f));

        ImGui::Begin("SeekBarWidget", nullptr, flags);

        // Reset the progress changed flag
        m_progressChanged = false;

        // Calculate current time based on progress
        float currentTime = m_currentProgress * m_duration;

        // Apply text color
        ImGui::PushStyleColor(ImGuiCol_Text, m_textColor);

        // First row layout: <current time> <seek bar> <end time>

        // 1. Display current time
        char currentTimeStr[32];
        FormatTime(currentTimeStr, sizeof(currentTimeStr), currentTime);

        float timeTextWidth = ImGui::CalcTextSize(currentTimeStr).x;
        ImGui::Text("%s", currentTimeStr);
        float currentTimeWidth = ImGui::GetItemRectSize().x;
        ImGui::SameLine();

        // 2. Main seek bar
        float seekBarWidth = ImGui::GetContentRegionAvail().x - 100.0f; // Reserve space for end time

        // Apply seek bar colors
        ImGui::PushStyleColor(ImGuiCol_SliderGrab, m_thumbColor);
        ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, m_thumbColor);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, m_trackColor);
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, m_trackColor);
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, m_trackColor);

        ImGui::SetNextItemWidth(seekBarWidth);
        ImGui::InvisibleButton("##CustomSeekBar", ImVec2(seekBarWidth, 10.0f));

        // Store the seek bar position and size
        ImVec2 sliderPos = ImGui::GetItemRectMin();
        sliderPos.y += 2;
        ImVec2 sliderSize = ImGui::GetItemRectSize();
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        // Handle interaction
        bool isActive = ImGui::IsItemActive();
        bool isHovered = ImGui::IsItemHovered();

        if (isActive)
        {
            // Calculate new progress based on mouse position
            float mouseX = ImGui::GetIO().MousePos.x;
            float relativeX = mouseX - sliderPos.x;
            float newProgress = std::clamp(relativeX / sliderSize.x, 0.0f, 1.0f);

            if (newProgress != m_currentProgress)
            {
                m_currentProgress = newProgress;
                m_progressChanged = true;
            }
        }

        // Draw background track
        drawList->AddRectFilled(
            sliderPos,
            ImVec2(sliderPos.x + sliderSize.x, sliderPos.y + sliderSize.y),
            m_trackColor
        );

        // Draw active part of the track (colored part showing progress)
        float progressWidth = m_currentProgress * sliderSize.x;
        drawList->AddRectFilled(
            sliderPos,
            ImVec2(sliderPos.x + progressWidth, sliderPos.y + sliderSize.y),
            m_activeTrackColor
        );

        // Draw fixed-size playhead indicator
        const float playheadWidth = 4.0f; // Width of the playhead in pixels
        const float playheadHeight = sliderSize.y + 6.0f; // Make slightly taller than the track
        const float playheadX = sliderPos.x + (m_currentProgress * sliderSize.x) - (playheadWidth / 2.0f);
        const float playheadY = sliderPos.y - 3.0f; // Offset to center vertically

        drawList->AddRectFilled(
            ImVec2(playheadX, playheadY),
            ImVec2(playheadX + playheadWidth, playheadY + playheadHeight),
            m_thumbColor // Using the same thumb color for the playhead
        );

        // Restore colors
        ImGui::PopStyleColor(5);

        // Draw chapter markers
        static float sz = 10.0f;
        ImGuiIO& io = ImGui::GetIO();

        // Check if mouse was released - reset dragging state
        if (m_draggingMarker && !io.MouseDown[ImGuiMouseButton_Left])
        {
            m_draggingMarker = false;
            m_draggedMarkerIndex = -1;
        }

        for (int i = 0; i < m_chapterMarkers.size(); i++)
        {
            const auto [time, name, color] = m_chapterMarkers[i];
            const float markerProgress = time / m_duration;
            const float markerX = sliderPos.x + (markerProgress * sliderSize.x);

            auto p1 = ImVec2(markerX, sliderPos.y - 4.0f);
            auto p2 = ImVec2(markerX + sz * 0.5f, sliderPos.y - 4.0f - sz * 0.5f);
            auto p3 = ImVec2(markerX - sz * 0.5f, (sliderPos.y) - 4.0f - sz * 0.5f);

            drawList->AddTriangleFilled(p1,
                                        p2,
                                        p3,
                                        ImGui::ColorConvertFloat4ToU32(color));

            ImRect markerRect(
                ImVec2(p3.x - 2.0f, p3.y),
                ImVec2(p2.x + 2.0f, sliderPos.y - 2.0f)
            );

            // Start dragging if mouse is pressed on this marker
            if (!m_draggingMarker &&
                io.MouseDown[ImGuiMouseButton_Left] &&
                ImGui::IsMouseHoveringRect(markerRect.Min, markerRect.Max) &&
                ImGui::IsMouseDragging(ImGuiMouseButton_Left, 3.0f))
            {
                m_draggingMarker = true;
                m_draggedMarkerIndex = i;
            }

            // if the mouse is just clicked on the marker, open a popup
            if (!m_draggingMarker &&
                ImGui::IsMouseReleased(ImGuiMouseButton_Left) &&
                ImGui::IsMouseHoveringRect(markerRect.Min, markerRect.Max))
            {
                // Check if this was actually a drag operation by measuring the drag distance
                ImVec2 dragDelta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left, 0.0f);
                float dragDistance = std::sqrt(dragDelta.x * dragDelta.x + dragDelta.y * dragDelta.y);

                // Only open popup if the drag distance is below a threshold
                // This prevents the popup from opening after a genuine drag operation
                const float clickThreshold = 3.0f; // Adjust this value as needed
                if (dragDistance < clickThreshold)
                {
                    m_selectedMarkerIndex = i;
                    ImGui::OpenPopup("EditMarkerPopup");
                }
            }

            // Update marker position if it's the one being dragged
            if (m_draggingMarker && m_draggedMarkerIndex == i)
            {
                // Calculate new marker position based on mouse position
                float mouseX = io.MousePos.x;
                float relativeX = mouseX - sliderPos.x;
                float newProgress = std::clamp(relativeX / sliderSize.x, 0.0f, 1.0f);
                float newTime = newProgress * m_duration;

                // Define minimum time distance between markers (either 1% of total duration or 0.5 seconds, whichever is greater)
                const float minTimeDistance = std::max(m_duration * 0.01f, 0.5f);

                // Update the marker position
                m_chapterMarkers[i].time = newTime;

                // Check for and resolve overlaps with other markers
                for (int j = 0; j < m_chapterMarkers.size(); j++)
                {
                    if (i == j)
                    {
                        continue;
                    }

                    float timeDiff = m_chapterMarkers[j].time - m_chapterMarkers[i].time;
                    if (std::abs(timeDiff) < minTimeDistance)
                    {
                        // Marker j is too close to marker i
                        if (timeDiff > 0)
                        {
                            // Marker j is to the right of marker i, push it further right
                            m_chapterMarkers[j].time = m_chapterMarkers[i].time + minTimeDistance;
                        }
                        else
                        {
                            // Marker j is to the left of marker i, push it further left
                            m_chapterMarkers[j].time = m_chapterMarkers[i].time - minTimeDistance;
                        }

                        // Ensure marker j stays within bounds
                        m_chapterMarkers[j].time = std::clamp(m_chapterMarkers[j].time, 0.0f, m_duration);
                    }
                }
            }

            // Display tooltip when hovering over marker
            if (ImGui::IsMouseHoveringRect(markerRect.Min, markerRect.Max) && !m_draggingMarker)
            {
                ImGui::BeginTooltip();
                ImGui::Text("%s (%s)", name.c_str(), FormatTimeString(time).c_str());
                ImGui::EndTooltip();
            }
        }

        // Hover tooltip for seek bar
        if (ImGui::IsItemHovered())
        {
            // Calculate time position based on mouse position
            ImVec2 mousePos = ImGui::GetIO().MousePos;
            float relativeX = mousePos.x - sliderPos.x;
            float hoverProgress = std::clamp(relativeX / sliderSize.x, 0.0f, 1.0f);
            float hoverTime = hoverProgress * m_duration;

            char hoverTimeStr[32];
            FormatTime(hoverTimeStr, sizeof(hoverTimeStr), hoverTime);

            ImGui::BeginTooltip();
            ImGui::Text("%s", hoverTimeStr);
            ImGui::EndTooltip();
        }

        float seekBarTotalWidth = ImGui::GetItemRectSize().x;
        ImGui::SameLine();

        char endTimeStr[32];
        FormatTime(endTimeStr, sizeof(endTimeStr), m_duration);
        ImGui::Text("%s", endTimeStr);
        float endTimeWidth = ImGui::GetItemRectSize().x;

        float totalFirstRowWidth = currentTimeWidth + seekBarTotalWidth + endTimeWidth;

        // <padding><play button centered> <padding> <M><S>
        ImGui::Dummy(ImVec2(0, 5));

        float windowWidth = ImGui::GetWindowWidth();

        float playButtonWidth = 30.0f;
        float playButtonX = (totalFirstRowWidth - playButtonWidth) * 0.5f;

        float buttonSize = 25.0f;
        float rightButtonsPadding = 50.0f;
        float rightButtonsStartX = windowWidth - (buttonSize * 2 + rightButtonsPadding);

        ImGui::SetCursorPosX(playButtonX);
        if (ImGui::Button(m_isPlaying ? "*" : ">", ImVec2(playButtonWidth, 0)))
        {
            m_isPlaying = !m_isPlaying;
        }

        ImGui::SameLine();
        ImGui::SetCursorPosX(rightButtonsStartX);

        if (ImGui::Button("M", ImVec2(buttonSize, 0)))
        {
            m_addingMarker = true;
            strcpy(m_newMarkerName, "");
            ImGui::OpenPopup("MarkerPopup");
        }
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::Text("Add marker at current position");
            ImGui::EndTooltip();
        }

        ImGui::SameLine();

        if (ImGui::Button("S", ImVec2(buttonSize, 0)))
        {
            ImGui::OpenPopup("SettingsPopup");
        }
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::Text("Settings");
            ImGui::EndTooltip();
        }

        RenderSettingsPopup();

        RenderMarkerPopup();

        RenderEditMarkerPopup();

        ImGui::PopStyleColor();

        if (m_progressChanged && m_progressJumpCallback)
        {
            m_progressJumpCallback(m_currentProgress);
        }

        ImGui::End();
    }

    // Enable or disable keyboard input handling
    void SetKeyboardInputHandling(bool enable)
    {
        m_handleKeyboardInput = enable;
    }

    // Get current keyboard input handling state
    bool GetKeyboardInputHandling() const
    {
        return m_handleKeyboardInput;
    }

    // Configure key bindings
    void SetKeyConfig(const KeyConfig& config)
    {
        m_keyConfig = config;
    }

    // Get current key configuration
    const KeyConfig& GetKeyConfig() const
    {
        return m_keyConfig;
    }

    void SetDuration(float duration)
    {
        m_duration = std::max(0.1f, duration);
    }

    void SetProgress(float progress)
    {
        float newProgress = std::clamp(progress, 0.0f, 1.0f);
        if (newProgress != m_currentProgress)
        {
            m_currentProgress = newProgress;
            m_progressChanged = true;
        }
    }

    float GetProgress() const
    {
        return m_currentProgress;
    }

    float GetCurrentTime() const
    {
        return m_currentProgress * m_duration;
    }

    bool IsPlaying() const
    {
        return m_isPlaying;
    }

    void SetPlaying(bool isPlaying)
    {
        if (isPlaying && ImGui::GetIO().WantTextInput)
        {
            return;
        }

        m_isPlaying = isPlaying;
    }

    float GetPlaybackSpeed() const
    {
        return m_playbackSpeed;
    }

    // Get the total duration in seconds
    float GetDuration() const
    {
        return m_duration;
    }

    void SetPlaybackSpeed(float speed)
    {
        m_playbackSpeed = std::clamp(speed, 0.25f, 3.0f);
    }

    void SetProgressJumpCallback(const ProgressJumpCallback& callback)
    {
        m_progressJumpCallback = callback;
    }

    void AddChapterMarker(float time, const std::string& name)
    {
        // Define minimum time distance between markers
        const float minTimeDistance = std::max(m_duration * 0.01f, 0.5f);

        // Clamp time to valid range
        float adjustedTime = std::clamp(time, 0.0f, m_duration);

        // Check for overlaps with existing markers and adjust if needed
        bool needsAdjustment = true;
        while (needsAdjustment)
        {
            needsAdjustment = false;
            for (const auto& marker : m_chapterMarkers)
            {
                if (std::abs(marker.time - adjustedTime) < minTimeDistance)
                {
                    // Adjust to avoid overlap - move slightly to the right
                    adjustedTime = marker.time + minTimeDistance;
                    if (adjustedTime > m_duration)
                    {
                        // If we reach the end, try to the left of the existing marker
                        adjustedTime = marker.time - minTimeDistance;
                    }
                    adjustedTime = std::clamp(adjustedTime, 0.0f, m_duration);
                    needsAdjustment = true;
                    break;
                }
            }
        }

        ChapterMarker marker;
        marker.time = adjustedTime;
        marker.name = name;
        m_chapterMarkers.push_back(marker);

        std::ranges::sort(m_chapterMarkers,
                          [] (const ChapterMarker& a, const ChapterMarker& b) {
                              return a.time < b.time;
                          });
    }

    void ClearChapterMarkers()
    {
        m_chapterMarkers.clear();
    }

    void SetTrackColor(ImU32 color)
    {
        m_trackColor = color;
    }

    void SetActiveTrackColor(ImU32 color)
    {
        m_activeTrackColor = color;
    }

    void SetThumbColor(ImU32 color)
    {
        m_thumbColor = color;
    }

    void SetChapterMarkerColor(ImU32 color)
    {
        m_chapterMarkerColor = color;
    }

    void SetTextColor(ImU32 color)
    {
        m_textColor = color;
    }

    static ImU32 CreateColor(float r, float g, float b, float a = 1.0f)
    {
        return IM_COL32(
            static_cast<int>(r * 255.0f),
            static_cast<int>(g * 255.0f),
            static_cast<int>(b * 255.0f),
            static_cast<int>(a * 255.0f)
        );
    }

private:
    // Handle keyboard inputs
    void HandleKeyboardInput()
    {
        // Skip keyboard handling if ImGui wants text input (e.g., when typing in a text field)
        if (ImGui::GetIO().WantTextInput)
            return;

        // Only process key presses, not continuous holding
        // Toggle play/pause with spacebar
        if (ImGui::IsKeyPressed(m_keyConfig.togglePlay, false))
        {
            SetPlaying(!m_isPlaying);
        }

        // R to reset to beginning
        if (ImGui::IsKeyPressed(m_keyConfig.reset, false))
        {
            SetProgress(0.0f);
        }
    }

    void RenderSettingsPopup()
    {
        if (ImGui::BeginPopup("SettingsPopup"))
        {
            ImGui::Text("Playback Speed");

            ImGui::SetNextItemWidth(150.0f);
            if (ImGui::SliderFloat("##PlaybackSpeed", &m_playbackSpeed, 0.25f, 2.0f, "%.2fx"))
            {
                // changed
            }

            if (ImGui::Button("0.5x"))
                m_playbackSpeed = 0.5f;
            ImGui::SameLine();
            if (ImGui::Button("1.0x"))
                m_playbackSpeed = 1.0f;
            ImGui::SameLine();
            if (ImGui::Button("1.5x"))
                m_playbackSpeed = 1.5f;
            ImGui::SameLine();
            if (ImGui::Button("2.0x"))
                m_playbackSpeed = 2.0f;

            ImGui::Separator();

            if (ImGui::CollapsingHeader("Keyboard Controls", ImGuiTreeNodeFlags_DefaultOpen))
            {
                // Checkbox to enable/disable keyboard controls
                ImGui::Checkbox("Enable Keyboard Controls", &m_handleKeyboardInput);

                // Only show key binding options if keyboard controls are enabled
                if (m_handleKeyboardInput)
                {
                    ImGui::Text("Key Bindings:");
                    ImGui::Indent();

                    // This is a simplified approach - in a real implementation,
                    // you'd want a proper key binding UI with detection of pressed keys
                    ImGui::Text("Play/Pause: Space");
                    ImGui::Text("Seek Left: Left Arrow");
                    ImGui::Text("Seek Right: Right Arrow");
                    ImGui::Text("Reset: R");

                    ImGui::Unindent();
                }
            }

            if (ImGui::CollapsingHeader("Appearance", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::ColorEdit3("Track Color", (float *) &m_trackColorEdit);
                if (ImGui::IsItemDeactivatedAfterEdit())
                {
                    m_trackColor = ImGui::ColorConvertFloat4ToU32(m_trackColorEdit);
                }

                ImGui::ColorEdit3("Active Track Color", (float *) &m_activeTrackColorEdit);
                if (ImGui::IsItemDeactivatedAfterEdit())
                {
                    m_activeTrackColor = ImGui::ColorConvertFloat4ToU32(m_activeTrackColorEdit);
                }

                ImGui::ColorEdit3("Thumb Color", (float *) &m_thumbColorEdit);
                if (ImGui::IsItemDeactivatedAfterEdit())
                {
                    m_thumbColor = ImGui::ColorConvertFloat4ToU32(m_thumbColorEdit);
                }

                ImGui::ColorEdit3("Chapter Marker Color", (float *) &m_chapterMarkerColorEdit);
                if (ImGui::IsItemDeactivatedAfterEdit())
                {
                    m_chapterMarkerColor = ImGui::ColorConvertFloat4ToU32(m_chapterMarkerColorEdit);
                }
            }

            ImGui::EndPopup();
        }
    }

    void RenderMarkerPopup()
    {
        if (ImGui::BeginPopup("MarkerPopup"))
        {
            ImGui::Text("Add Marker at %s", FormatTimeString(GetCurrentTime()).c_str());
            ImGui::Separator();

            ImGui::Text("Marker Name:");
            ImGui::SetNextItemWidth(200.0f);
            ImGui::InputText("##MarkerName", m_newMarkerName, sizeof(m_newMarkerName));

            ImGui::Spacing();

            float buttonWidth = 96.0f;
            if (ImGui::Button("Add", ImVec2(buttonWidth, 0)))
            {
                // Add the marker at the current position
                if (strlen(m_newMarkerName) > 0)
                {
                    AddChapterMarker(GetCurrentTime(), m_newMarkerName);
                    m_addingMarker = false;
                    ImGui::CloseCurrentPopup();
                }
            }

            ImGui::SameLine();

            if (ImGui::Button("Cancel", ImVec2(buttonWidth, 0)))
            {
                m_addingMarker = false;
                ImGui::CloseCurrentPopup();
            }


            ImGui::EndPopup();
        }
    }

    void RenderEditMarkerPopup()
    {
        if (ImGui::BeginPopup("EditMarkerPopup"))
        {
            ImGui::Text("Edit Marker");
            ImGui::Separator();

            ImGui::Text("Marker Name:");
            ImGui::SetNextItemWidth(200.0f);
            ImGui::InputText("##EditMarkerName", m_chapterMarkers[m_selectedMarkerIndex].name.data(), m_chapterMarkers[m_selectedMarkerIndex].name.size());

            ImGui::Spacing();


            ImGui::ColorEdit3("Marker Color", (float*) &m_chapterMarkers[m_selectedMarkerIndex].color);


            float buttonWidth = 96.0f;


            if (ImGui::Button("Delete", ImVec2(buttonWidth, 0)))
            {
                // Delete the marker
                m_chapterMarkers.erase(m_chapterMarkers.begin() + m_selectedMarkerIndex);
                m_selectedMarkerIndex = -1;
                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();

            if (ImGui::Button("Cancel", ImVec2(buttonWidth, 0)))
            {
                m_selectedMarkerIndex = -1;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }

    void FormatTime(char* buffer, size_t bufferSize, float timeInSeconds)
    {
        int minutes = static_cast<int>(timeInSeconds) / 60;
        float seconds = timeInSeconds - (minutes * 60.0f);
        snprintf(buffer, bufferSize, "%02d:%05.2f", minutes, seconds);
    }

    // Returns a formatted time string
    std::string FormatTimeString(float timeInSeconds)
    {
        char buffer[32];
        FormatTime(buffer, sizeof(buffer), timeInSeconds);
        return std::string(buffer);
    }

private:
    float m_currentProgress;
    float m_duration;
    bool m_isPlaying;
    float m_playbackSpeed;
    bool m_progressChanged;
    bool m_handleKeyboardInput;
    KeyConfig m_keyConfig;

    std::vector<ChapterMarker> m_chapterMarkers;
    bool m_addingMarker;
    char m_newMarkerName[64];

    ImU32 m_trackColor;
    ImU32 m_activeTrackColor;
    ImU32 m_thumbColor;
    ImU32 m_chapterMarkerColor;
    ImU32 m_textColor;

    ImVec4 m_trackColorEdit = ImVec4(0.39f, 0.39f, 0.39f, 1.0f);
    ImVec4 m_activeTrackColorEdit = ImVec4(0.2f, 0.6f, 0.8f, 1.0f);
    ImVec4 m_thumbColorEdit = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
    ImVec4 m_chapterMarkerColorEdit = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);

    bool m_draggingMarker = false;
    int m_draggedMarkerIndex = -1;
    int m_selectedMarkerIndex = -1;

    ProgressJumpCallback m_progressJumpCallback;
};

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

int main()
{
    g_logger.set_level(LogLevel::Debug);
    g_logger.add_sink(FileSink("app.log"),
                      "[{timestamp:%Y-%m-%d %H:%M:%S}] [{level}] {message}",
                      LogLevel::Debug
    );

    auto window = sf::RenderWindow(sf::VideoMode({1920u, 1080u}), "CMake SFML Project");
    window.setFramerateLimit(144);
    if (!ImGui::SFML::Init(window))
        return -1;

    embraceTheDarkness();

    sf::Clock clock;
    sf::Clock realTimeClock; // For tracking actual time

    // Create seek bar widget
    ImGuiSeekBarWidget seekBar;

    seekBar.SetDuration(300.0f);

    auto positionCallback = [] (float position) {
        std::cout << "Position changed to: " << position << " seconds" << std::endl;
    };
    seekBar.SetProgressJumpCallback([&positionCallback] (float progress) {
        float timeInSeconds = progress * 300.0f; // Calculate actual time
        positionCallback(timeInSeconds);
    });


    seekBar.AddChapterMarker(30.0f, "Introduction");
    seekBar.AddChapterMarker(75.0f, "Baseline Recording");
    seekBar.AddChapterMarker(120.0f, "First Stimulus");
    seekBar.AddChapterMarker(180.0f, "Rest Period");
    seekBar.AddChapterMarker(210.0f, "Second Stimulus");
    seekBar.AddChapterMarker(270.0f, "Final Assessment");

    seekBar.SetTrackColor(ImGuiSeekBarWidget::CreateColor(0.2f, 0.2f, 0.25f));
    seekBar.SetActiveTrackColor(ImGuiSeekBarWidget::CreateColor(0.1f, 0.6f, 0.8f));
    seekBar.SetThumbColor(ImGuiSeekBarWidget::CreateColor(0.9f, 0.9f, 0.9f));
    seekBar.SetChapterMarkerColor(ImGuiSeekBarWidget::CreateColor(1.0f, 0.7f, 0.2f));

    float lastFrameTime = realTimeClock.getElapsedTime().asSeconds();

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            ImGui::SFML::ProcessEvent(window, *event);

            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }

        }

        // Get the current time
        float currentTime = realTimeClock.getElapsedTime().asSeconds();
        float deltaTime = currentTime - lastFrameTime;
        lastFrameTime = currentTime;

        ImGui::SFML::Update(window, clock.restart());

        // Show main demo window for reference
        ImGui::ShowDemoWindow();

        // Create a demo control window
        ImGui::Begin("Seek Bar Demo Controls");

        ImGui::Text("EEG Recording Playback");
        ImGui::Separator();

        ImGui::Text("Keyboard Shortcuts:");
        ImGui::BulletText("Space: Toggle Play/Pause");
        ImGui::BulletText("F: Toggle Fullscreen Mode");
        ImGui::BulletText("A: Toggle Auto-Advance (Demo)");
        ImGui::BulletText("Left/Right Arrows: Small Jumps");
        ImGui::BulletText("R: Reset to Beginning");

        ImGui::Separator();

        if (ImGui::Button("Add Random Marker"))
        {
            // Add a marker at a random position
            float randomTime = static_cast<float>(rand() % 300);
            char markerName[32];
            sprintf(markerName, "Event #%d", rand() % 100);
            seekBar.AddChapterMarker(randomTime, markerName);
        }

        ImGui::SameLine();

        if (ImGui::Button("Clear All Markers"))
        {
            seekBar.ClearChapterMarkers();
        }

        static float speedOverride = 1.0f;
        ImGui::SliderFloat("Playback Speed", &speedOverride, 0.25f, 3.0f, "%.2fx");
        if (ImGui::Button("Apply Speed"))
        {
            // This demonstrates how you could programmatically set the speed
            // instead of using the built-in controls
            seekBar.SetPlaybackSpeed(speedOverride);
        }

        ImGui::End();

        // Custom ImGui window to display current status
        ImGui::Begin("Seek Bar Status");
        ImGui::Text("Current Progress: %.2f%%", seekBar.GetProgress() * 100.0f);
        ImGui::Text("Current Time: %.2f seconds", seekBar.GetCurrentTime());
        ImGui::Text("Playback Speed: %.2fx", seekBar.GetPlaybackSpeed());
        ImGui::Text("Playback State: %s", seekBar.IsPlaying() ? "Playing" : "Paused");
        ImGui::End();

        // Render the seek bar widget
        seekBar.Render();

        // Update progress if playing or auto-advancing
        if (seekBar.IsPlaying())
        {
            float progressDelta = deltaTime / seekBar.GetDuration() * seekBar.GetPlaybackSpeed();
            seekBar.SetProgress(seekBar.GetProgress() + progressDelta);

            // For demo purposes, loop back to beginning when reaching the end
            if (seekBar.GetProgress() >= 1.0f)
            {
                seekBar.SetProgress(0.0f);
            }
        }

        window.clear();
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}
