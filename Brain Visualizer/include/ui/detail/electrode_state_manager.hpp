#pragma once

#include <array>

#include <electrode/electrode_set.hpp>
#include <analysis/batch_analyzer.hpp>

class ElectrodeStateManager
{
public:
    ElectrodeStateManager(brainviz::electrode::ElectrodeSet& electrodeSet, brainviz::analysis::BatchAnalyzer& analyzer);

    ~ElectrodeStateManager();

    // Update state based on time and animation speed
    void update(float deltaTime, float animationSpeed);

    [[nodiscard]] size_t get_frame_index() const;

    [[nodiscard]] size_t get_max_frame_index() const;

    [[nodiscard]] brainviz::data::FrequencyBand get_selected_band() const;

    [[nodiscard]] bool is_single_band_mode() const;

    // used in rendering
    struct ElectrodeVisualizationData
    {
        std::array<float, 5> radii; // interpolated radii for each band
        std::array<float, 5> alphas; // interpolated alphas for each band
    };

    [[nodiscard]] const tsl::robin_map<int, ElectrodeVisualizationData>&
    get_visualization_data() const;

private:
    // state tracking for lerping
    struct ElectrodeState
    {
        std::array<float, 5> previous_radii = {0.0f};
        std::array<float, 5> current_radii = {0.0f};
        std::array<float, 5> previous_alphas = {0.0f};
        std::array<float, 5> current_alphas = {0.0f};
    };


    // TODO: convert both of these to vector
    tsl::robin_map<int, ElectrodeState> m_electrodeStates;
    tsl::robin_map<int, ElectrodeVisualizationData> m_visualizationData;

    // state tracking
    brainviz::electrode::ElectrodeSet& m_electrodeSet;
    brainviz::analysis::BatchAnalyzer& m_analyzer;
    brainviz::data::FrequencyBand m_selectedBand = brainviz::data::FrequencyBand::Alpha;
    bool m_showOnlySelectedBand = false;

    size_t m_windowSize;
    size_t m_hopSize;
    size_t m_frameIndex;
    size_t m_timeIndex;

    float m_timeSinceLastFrame = 0.0f;
    float m_interpolationProgress = 1.0f;
    float m_interpolationDuration = 0.20f;

    void advance_frame();

    void update_electrode_states();

    [[nodiscard]] size_t compute_time_index(size_t desired_frame_index) const;

    void update_visualization_data();

    // event handlers
    void handle_band_selected(const brainviz::data::FrequencyBand band)
    {
        const brainviz::data::FrequencyBand previousBand = m_selectedBand;

        m_selectedBand = band;

        if (previousBand != band)
        {
            m_interpolationProgress = 0.0f;
        }
    }

    void handle_mode_changed(bool singleBandMode)
    {
        // reset only if the mode actually changed...
        if (m_showOnlySelectedBand != singleBandMode)
        {
            m_showOnlySelectedBand = singleBandMode;

            m_interpolationProgress = 0.0f;
        }
    }

    FORCE_INLINE static float lerp(const float p0, const float p1, float t)
    {
        t = std::max(0.0f, std::min(1.0f, t));
        return p0 * (1.0f - t) + p1 * t;
    }
};
