#include <ui/frequency_band_selector.hpp>

void FrequencyBandSelector::render()
{
	using namespace brainviz;
	ImGui::Begin("EEG Band Controls");

	// truly gods most intuitive library
	if (ImGui::RadioButton("Delta (0.5-4 Hz)", m_selectedBand == data::FrequencyBand::Delta))
	{
		m_selectedBand = data::FrequencyBand::Delta;
		FrequencyBandSelectedEvent::post(m_selectedBand);
	}
	if (ImGui::RadioButton("Theta (4-8 Hz)", m_selectedBand == data::FrequencyBand::Theta))
	{
		m_selectedBand = data::FrequencyBand::Theta;
		FrequencyBandSelectedEvent::post(m_selectedBand);
	}
	if (ImGui::RadioButton("Alpha (8-13 Hz)", m_selectedBand == data::FrequencyBand::Alpha))
	{
		m_selectedBand = data::FrequencyBand::Alpha;
		FrequencyBandSelectedEvent::post(m_selectedBand);
	}
	if (ImGui::RadioButton("Beta (13-30 Hz)", m_selectedBand == data::FrequencyBand::Beta))
	{
		m_selectedBand = data::FrequencyBand::Beta;
		FrequencyBandSelectedEvent::post(m_selectedBand);
	}
	if (ImGui::RadioButton("Gamma (30+ Hz)", m_selectedBand == data::FrequencyBand::Gamma))
	{
		m_selectedBand = data::FrequencyBand::Gamma;
		FrequencyBandSelectedEvent::post(m_selectedBand);
	}

	ImGui::Separator();

	const bool old_value = m_showOnlySelectedBand;
	if (ImGui::Checkbox("Show only selected band", &m_showOnlySelectedBand))
	{
		if (old_value != m_showOnlySelectedBand)
		{
			VisualizationModeChangedEvent::post(m_showOnlySelectedBand);
		}
	}

	ImGui::SliderFloat("Animation Speed", &m_animationSpeed, 0.1f, 5.0f, "%.1fx");

	ImGui::Text("Frame: %zu/%zu", m_currentFrame, m_maxFrames);

	ImGui::End();
}
