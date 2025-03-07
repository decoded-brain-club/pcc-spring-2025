#pragma once

#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <string>
#include <vector>

#include <event/event_system.hpp>
#include <analysis/batch_analyzer.hpp>
#include <electrode/electrode_set.hpp>

EVENT_DEF(FrequencyBandSelectedEvent, brainviz::data::FrequencyBand);

EVENT_DEF(VisualizationModeChangedEvent, bool);

class FrequencyBandSelector
{
public:
    void render();

    [[nodiscard]] brainviz::data::FrequencyBand get_selected_band() const
    {
        return m_selectedBand;
    }

    [[nodiscard]] bool is_single_band_mode() const
    {
        return m_showOnlySelectedBand;
    }

    [[nodiscard]] float get_animation_speed() const
    {
        return m_animationSpeed;
    }

    void set_frame_info(const size_t current, const size_t max)
    {
        m_currentFrame = current;
        m_maxFrames = max;
    }

private:
    brainviz::data::FrequencyBand m_selectedBand = brainviz::data::FrequencyBand::Alpha;
    bool m_showOnlySelectedBand = false;
    float m_animationSpeed = 1.0f;
    size_t m_currentFrame = 0;
    size_t m_maxFrames = 0;
};
