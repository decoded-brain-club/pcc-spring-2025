#include <cmath>
#include <algorithm>
#include <stdexcept>

#include <fmt/format.h>

#include <logging/logger.hpp>
#include <analysis/batch_analyzer.hpp>

namespace brainviz::analysis
{
    size_t BatchAnalyzer::round_to_power_of_2(const size_t value)
    {
        if (value == 0)
            return 1;

        size_t next_power = 1;
        while (next_power < value)
        {
            next_power <<= 1;
        }

        const size_t prev_power = next_power >> 1;

        if (next_power - value < value - prev_power)
        {
            return next_power;
        }

        return prev_power;
    }

    BatchAnalyzer::BatchAnalyzer(
        const data::EEGData& eeg_data,
        const size_t window_size,
        const double overlap_percentage)
        : m_eeg_data(eeg_data),
          m_sampling_rate(eeg_data.m_samplingRate)
    {
        m_window_size = round_to_power_of_2(window_size);

        m_hop_size = static_cast<size_t>(m_window_size * (100.0 - overlap_percentage) / 100.0);

        if (m_hop_size < 1)
        {
            m_hop_size = 1;
        }

        g_logger.info("Initialized frequency analyzer with window size {} (power of 2)", m_window_size);
        g_logger.info("Using hop size {} ({:.1f}% overlap)", m_hop_size,
                      100.0 * (1.0 - static_cast<double>(m_hop_size) / m_window_size));
        g_logger.info("Frequency resolution: {:.3f} Hz", get_frequency_resolution());
    }

    void BatchAnalyzer::process_all_channels()
    {
        for (const auto& channel_name : m_eeg_data.get_channel_names())
        {
            process_channel(channel_name);
        }
    }

    void BatchAnalyzer::process_channel(const std::string_view channel_name)
    {
        const auto& raw_data = m_eeg_data.get_channel(channel_name);

        const size_t window_size = m_window_size;
        const size_t hop_size = m_hop_size;
        const size_t num_frames = (raw_data.size() > window_size) ? (raw_data.size() - window_size) / hop_size + 1 : 1;

        BandAmplitudes band_amplitudes;
        band_amplitudes.delta.resize(num_frames);
        band_amplitudes.theta.resize(num_frames);
        band_amplitudes.alpha.resize(num_frames);
        band_amplitudes.beta.resize(num_frames);
        band_amplitudes.gamma.resize(num_frames);

        const kfr::dft_plan<double> dft(window_size);
        kfr::univector<uint8_t> temp(dft.temp_size);

        for (size_t frame = 0; frame < num_frames; ++frame)
        {
            const size_t start_idx = frame * hop_size;

            kfr::univector<double> frame_data(window_size);
            for (size_t i = 0; i < window_size && (start_idx + i) < raw_data.size(); ++i)
            {
                frame_data[i] = raw_data[start_idx + i];
            }

            frame_data = frame_data * kfr::window_hann(window_size);

            kfr::univector<kfr::complex<double> > complex_data(window_size);
            for (size_t i = 0; i < window_size; ++i)
            {
                complex_data[i] = kfr::complex<double>(frame_data[i], 0.0);
            }

            kfr::univector<kfr::complex<double> > fft_result(window_size);
            dft.execute(fft_result, complex_data, temp, false);

            kfr::univector<double> power_spectrum(window_size / 2 + 1);
            for (size_t i = 0; i < power_spectrum.size(); ++i)
            {
                power_spectrum[i] = std::norm(fft_result[i]);
            }

            const double freq_resolution = m_sampling_rate / window_size;

            band_amplitudes.delta[frame] = calculate_band_amplitude(power_spectrum, freq_resolution,
                                                                    data::FrequencyRange::DELTA_MIN,
                                                                    data::FrequencyRange::DELTA_MAX);

            band_amplitudes.theta[frame] = calculate_band_amplitude(power_spectrum, freq_resolution,
                                                                    data::FrequencyRange::THETA_MIN,
                                                                    data::FrequencyRange::THETA_MAX);

            band_amplitudes.alpha[frame] = calculate_band_amplitude(power_spectrum, freq_resolution,
                                                                    data::FrequencyRange::ALPHA_MIN,
                                                                    data::FrequencyRange::ALPHA_MAX);

            band_amplitudes.beta[frame] = calculate_band_amplitude(power_spectrum, freq_resolution,
                                                                   data::FrequencyRange::BETA_MIN,
                                                                   data::FrequencyRange::BETA_MAX);

            band_amplitudes.gamma[frame] = calculate_band_amplitude(power_spectrum, freq_resolution,
                                                                    data::FrequencyRange::GAMMA_MIN,
                                                                    data::FrequencyRange::GAMMA_MAX);
        }

        m_channel_amplitudes[std::string(channel_name)] = std::move(band_amplitudes);
    }

    const kfr::univector<double>& BatchAnalyzer::get_band_amplitude(
        const data::FrequencyBand band,
        const std::string_view channel_name) const
    {
        const auto it = m_channel_amplitudes.find(std::string(channel_name));
        if (it == m_channel_amplitudes.end())
        {
            throw std::runtime_error(fmt::format("Channel not processed: {}", channel_name));
        }

        const auto& [delta,
            theta,
            alpha,
            beta,
            gamma] = it->second;

        switch (band)
        {
            case data::FrequencyBand::Delta:
                return delta;
            case data::FrequencyBand::Theta:
                return theta;
            case data::FrequencyBand::Alpha:
                return alpha;
            case data::FrequencyBand::Beta:
                return beta;
            case data::FrequencyBand::Gamma:
                return gamma;
            default:
                throw std::invalid_argument("Invalid frequency band");
        }
    }


    size_t BatchAnalyzer::get_max_frame_index() const
    {
        if (m_channel_amplitudes.empty())
        {
            return 0;
        }

        const auto& first_channel = m_channel_amplitudes.begin()->second;

        if (first_channel.delta.empty())
        {
            return 0;
        }

        return first_channel.delta.size() - 1;
    }

    size_t BatchAnalyzer::time_index_to_frame(const size_t time_index) const
    {
        const size_t window_size = m_window_size;
        const size_t hop_size = m_hop_size;

        if (time_index < window_size / 2)
        {
            return 0;
        }

        size_t frame = (time_index - window_size / 2) / hop_size;
        return std::min(frame, get_max_frame_index());
    }

    data::FrequencyBand BatchAnalyzer::get_dominant_band(
        const std::string_view channel_name,
        const size_t time_index) const
    {
        const size_t frame_index = time_index_to_frame(time_index);

        double max_amplitude = -1.0;
        auto dominant_band = data::FrequencyBand::Delta;

        for (int band_idx = 0; band_idx < 5; ++band_idx)
        {
            const auto curr_band = static_cast<data::FrequencyBand>(band_idx);
            const auto& band_amplitudes = get_band_amplitude(curr_band, channel_name);

            if (frame_index < band_amplitudes.size() && band_amplitudes[frame_index] > max_amplitude)
            {
                max_amplitude = band_amplitudes[frame_index];
                dominant_band = curr_band;
            }
        }

        return dominant_band;
    }

    std::array<BatchAnalyzer::VisualizationInfo, 5> BatchAnalyzer::get_visualization_info(
        const std::string_view channel_name,
        const size_t time_index) const
    {
        const size_t frame_index = time_index_to_frame(time_index);

        std::array<VisualizationInfo, 5> result{};

        double max_amplitude = 0.0;

        for (int band_idx = 0; band_idx < 5; ++band_idx)
        {
            const auto curr_band = static_cast<data::FrequencyBand>(band_idx);
            const auto& band_amplitudes = get_band_amplitude(curr_band, channel_name);
            if (frame_index < band_amplitudes.size())
            {
                max_amplitude = std::max(max_amplitude, band_amplitudes[frame_index]);
            }
        }

        for (int band_idx = 0; band_idx < 5; ++band_idx)
        {
            const auto curr_band = static_cast<data::FrequencyBand>(band_idx);
            const auto& band_amplitudes = get_band_amplitude(curr_band, channel_name);

            const double amplitude = (frame_index < band_amplitudes.size()) ? band_amplitudes[frame_index] : 0.0;

            result[band_idx].band = curr_band;
            result[band_idx].radius_multiplier = calculate_radius_multiplier(amplitude, max_amplitude);
            result[band_idx].transparency = calculate_transparency(amplitude, max_amplitude);
        }

        return result;
    }

    double BatchAnalyzer::calculate_band_amplitude(
        const kfr::univector<double>& power_spectrum,
        const double freq_resolution,
        const double min_freq,
        const double max_freq)
    {
        double band_power = 0.0;

        const auto min_bin = static_cast<size_t>(std::ceil(min_freq / freq_resolution));
        const auto max_bin = static_cast<size_t>(std::floor(max_freq / freq_resolution));

        for (size_t i = min_bin; i <= max_bin && i < power_spectrum.size(); ++i)
        {
            band_power += power_spectrum[i];
        }

        return std::sqrt(band_power);
    }

    double BatchAnalyzer::calculate_radius_multiplier(const double amplitude, const double max_amplitude)
    {
        if (max_amplitude <= 0.0)
            return 0.0;

        const double normalized = amplitude / max_amplitude;

        constexpr double base_radius = 0.2;
        constexpr double max_radius = 1.0;

        return base_radius + normalized * (max_radius - base_radius);
    }

    double BatchAnalyzer::calculate_transparency(const double amplitude, const double max_amplitude)
    {
        if (max_amplitude <= 0.0)
            return 0.0;

        const double normalized = amplitude / max_amplitude;

        constexpr double min_transparency = 0.2;
        constexpr double max_transparency = 1.0;

        return min_transparency + normalized * (max_transparency - min_transparency);
    }
} // namespace brainviz::analysis
