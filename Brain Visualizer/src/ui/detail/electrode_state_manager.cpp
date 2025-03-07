#include <ui/detail/electrode_state_manager.hpp>

#include <electrode/electrode_set.hpp>
#include <ui/frequency_band_selector.hpp>

ElectrodeStateManager::ElectrodeStateManager(brainviz::electrode::ElectrodeSet& electrodeSet, brainviz::analysis::BatchAnalyzer& analyzer)
    : m_electrodeSet(electrodeSet),
      m_analyzer(analyzer),
      m_windowSize(analyzer.get_window_size()),
      m_hopSize(analyzer.get_hop_size()),
      m_frameIndex(0),
      m_timeIndex(compute_time_index(0))
{

    FrequencyBandSelectedEvent::subscribe([this](const brainviz::data::FrequencyBand band) {
        handle_band_selected(band);
    });

    VisualizationModeChangedEvent::subscribe([this](const bool singleBandMode) {
        handle_mode_changed(singleBandMode);
    });

    update_electrode_states();
    update_visualization_data();
}

ElectrodeStateManager::~ElectrodeStateManager()
{
    FrequencyBandSelectedEvent::unsubscribe();
    VisualizationModeChangedEvent::unsubscribe();
}

void ElectrodeStateManager::update(const float deltaTime, const float animationSpeed)
{
    m_timeSinceLastFrame += deltaTime;
    if (m_timeSinceLastFrame > (0.05f / animationSpeed))
    {
        advance_frame();
        m_timeSinceLastFrame = 0.0f;
    }

    m_interpolationProgress = std::min(1.0f, m_interpolationProgress + deltaTime / m_interpolationDuration);

    update_visualization_data();
}

void ElectrodeStateManager::advance_frame()
{
    m_frameIndex = (m_frameIndex + 1) % m_analyzer.get_max_frame_index();
    m_timeIndex = compute_time_index(m_frameIndex);

    update_electrode_states();

    m_interpolationProgress = 0.0f;
}

[[nodiscard]] size_t ElectrodeStateManager::get_frame_index() const
{
    return m_frameIndex;
}

[[nodiscard]] size_t ElectrodeStateManager::get_max_frame_index() const
{
    return m_analyzer.get_max_frame_index();
}

[[nodiscard]] bool ElectrodeStateManager::is_single_band_mode() const
{
    return m_showOnlySelectedBand;
}

[[nodiscard]] brainviz::data::FrequencyBand ElectrodeStateManager::get_selected_band() const
{
    return m_selectedBand;
}

[[nodiscard]] size_t ElectrodeStateManager::compute_time_index(const size_t desired_frame_index) const
{
    if (desired_frame_index == 0)
    {
        return 0;
    }
    else
    {
        return (m_windowSize / 2) + (desired_frame_index * m_hopSize);
    }
}

void ElectrodeStateManager::update_electrode_states()
{
    for (const auto& electrode : m_electrodeSet)
    {
        const int id = electrode.id();
        const auto name = electrode.name();

        try
        {
            const auto visualizationInfo = m_analyzer.get_visualization_info(name, m_timeIndex);

            auto& [previous_radii,
                current_radii,
                previous_alphas,
                current_alphas] = m_electrodeStates[id];

            for (int i = 0; i < 5; ++i)
            {
                previous_radii[i] = current_radii[i];
                current_radii[i] = static_cast<float>(visualizationInfo[i].radius_multiplier);

                previous_alphas[i] = current_alphas[i];
                current_alphas[i] = static_cast<float>(visualizationInfo[i].transparency);
            }
        }
        catch (const std::exception& e)
        {
            continue;
        }
    }
}

[[nodiscard]] const tsl::robin_map<int, ElectrodeStateManager::ElectrodeVisualizationData>&
    ElectrodeStateManager::get_visualization_data() const
{
    return m_visualizationData;
}

void ElectrodeStateManager::update_visualization_data()
{
    for (const auto& [id, state] : m_electrodeStates)
    {
        auto& [radii, alphas] = m_visualizationData[id];

        for (int i = 0; i < 5; ++i)
        {
            radii[i] = lerp(
                state.previous_radii[i],
                state.current_radii[i],
                m_interpolationProgress
            );

            alphas[i] = lerp(
                state.previous_alphas[i],
                state.current_alphas[i],
                m_interpolationProgress
            );
        }
    }
}