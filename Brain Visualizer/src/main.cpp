#include <algorithm>
#include <memory>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <data/interface.hpp>
#include <data/json_file_source.hpp>
#include <analysis/batch_analyzer.hpp>
#include <logging/logger.hpp>

// TODO: sightem add crash handler
// TODO: sightem use logger instead of fmt::print
// TODO: drop exceptions in favor of std::expected

using namespace std::string_view_literals;

std::unique_ptr<brainviz::data::EEGDataSource> create_data_source(const std::string_view source_type,
                                                                  const std::string_view path)
{
    if (source_type == "file"sv)
    {
        return std::make_unique<brainviz::data::JSONFileSource>(path);
    }
    // future impls can be added here:
    // else if (source_type == "stream"sv)
    // {
    //     return std::make_unique<brainviz::data::StreamingEEGSource>(path);
    // }

    throw std::runtime_error(fmt::format("Unknown data source type: {}", source_type));
}

void preview_channel(const std::string_view channel_name, const std::vector<double>& data,
                     const size_t preview_size = 5)
{
    fmt::print("  {}: [", channel_name);

    size_t count = std::min(preview_size, data.size());
    for (size_t i = 0; i < count; ++i)
    {
        fmt::print("{:.4f}", data[i]);
        if (i < count - 1)
        {
            fmt::print(", ");
        }
    }

    if (data.size() > preview_size)
    {
        fmt::print(", ... ({} more values)", data.size() - preview_size);
    }

    fmt::print("]\n");
}

std::string_view frequency_band_to_string(const brainviz::data::FrequencyBand band)
{
    switch (band)
    {
        case brainviz::data::FrequencyBand::Delta:
            return "Delta";
        case brainviz::data::FrequencyBand::Theta:
            return "Theta";
        case brainviz::data::FrequencyBand::Alpha:
            return "Alpha";
        case brainviz::data::FrequencyBand::Beta:
            return "Beta";
        case brainviz::data::FrequencyBand::Gamma:
            return "Gamma";
        default:
            return "Unknown";
    }
}

int main()
{
    g_logger.set_level(LogLevel::Debug);

    g_logger.add_sink(FileSink("app.log"), //TODO: FIX THIS!! proper directory for logs. ideally all initialization code should be moved out of main into a separate function
                      "[{timestamp:%Y-%m-%d %H:%M:%S}] [{level}] {message}",
                      LogLevel::Debug
    );

    try
    {
        constexpr auto source_type = "file"sv;
        constexpr auto file_path = "eeg_data_named_channels.json"sv;

        fmt::print("Loading EEG data from: {} using {} source\n", file_path, source_type);

        const auto data_source = create_data_source(source_type, file_path);

        if (!data_source->is_data_available())
        {
            g_logger.error("Data not available from source: {}", data_source->get_source_name());
            return 1;
        }

        if (!data_source->open())
        {
            fmt::print(stderr, "Error: Failed to open the data source\n");
            return 1;
        }

        std::unique_ptr<brainviz::data::EEGData> eeg_data;
        try
        {
            eeg_data = data_source->load_data();
        }
        catch (const std::exception& e)
        {
            fmt::print(stderr, "Error loading EEG data: {}\n", e.what());
            return 1;
        }

        fmt::print("\nEEG Data Information:\n");
        fmt::print("--------------------\n");
        fmt::print("Source: {}\n", data_source->get_source_name());
        fmt::print("Number of channels: {}\n", eeg_data->get_channel_names().size());
        fmt::print("Samples per channel: {}\n", eeg_data->get_sample_count());
        fmt::print("Sampling rate: {:.1f} Hz\n", eeg_data->m_samplingRate);

        double duration_sec = eeg_data->get_sample_count() / eeg_data->m_samplingRate;
        fmt::print("Recording duration: {:.2f} seconds ({:.2f} minutes)\n",
                   duration_sec, duration_sec / 60.0);

        fmt::print("\nChannel Previews (first 5 samples):\n");
        fmt::print("----------------------------------\n");

        auto channel_names = eeg_data->get_channel_names();
        std::sort(channel_names.begin(), channel_names.end());

        for (const auto& channel_name : channel_names)
        {
            try
            {
                const auto& channel_data = eeg_data->get_channel(channel_name);
                preview_channel(channel_name, channel_data);
            }
            catch (const std::exception& e)
            {
                fmt::print(stderr, "  {}: Error accessing data: {}\n", channel_name, e.what());
            }
        }

        if (!channel_names.empty())
        {
            const auto& first_channel = eeg_data->get_channel(channel_names[0]);

            if (!first_channel.empty())
            {
                double sum = 0.0;
                double min = first_channel[0];
                double max = first_channel[0];

                for (const auto& value : first_channel)
                {
                    sum += value;
                    min = std::min(min, value);
                    max = std::max(max, value);
                }

                double avg = sum / first_channel.size();

                fmt::print("\nBasic Statistics for Channel {}:\n", channel_names[0]);
                fmt::print("-----------------------------------\n");
                fmt::print("  Min value: {}\n", min);
                fmt::print("  Max value: {}\n", max);
                fmt::print("  Average value: {}\n", avg);
                fmt::print("  Value range: {}\n", max - min);
            }
        }

        fmt::print("\nPerforming frequency analysis...\n");
        brainviz::analysis::BatchAnalyzer analyzer(*eeg_data, 256, 75.0);

        if (!channel_names.empty())
        {
            const auto& channel_to_analyze = channel_names[0];
            fmt::print("Analyzing frequency bands for channel: {}\n", channel_to_analyze);

            try
            {
                analyzer.process_channel(channel_to_analyze);

                const auto max_frame = analyzer.get_max_frame_index();
                const size_t hop_size = analyzer.get_hop_size();

                const std::array<size_t, 3> sample_times = {
                    0, // beginning
                    (max_frame > 0) ? (max_frame / 2) * hop_size : 0, // midle of processed frames
                    (max_frame > 0) ? max_frame * hop_size : 0 // end of processed frames
                };


                for (size_t time_idx : sample_times)
                {
                    // conv from sample index to time in seconds
                    double time_sec = time_idx / eeg_data->m_samplingRate;



                    fmt::print("\nFrequency analysis at time {:.2f}s (sample {})\n", time_sec, time_idx);
                    fmt::print("------------------------------------------------\n");

                    auto dominant_band = analyzer.get_dominant_band(channel_to_analyze, time_idx);
                    fmt::print("Dominant frequency band: {}\n", frequency_band_to_string(dominant_band));

                    fmt::print("\nVisualization parameters:\n");
                    auto viz_info = analyzer.get_visualization_info(channel_to_analyze, time_idx);

                    for (const auto& info : viz_info)
                    {
                        fmt::print("  {}: radius = {:.2f}, transparency = {:.2f}\n",
                                   frequency_band_to_string(info.band),
                                   info.radius_multiplier,
                                   info.transparency);
                    }
                }

                fmt::print("\nAverage amplitudes across all time points:\n");
                fmt::print("----------------------------------------\n");

                for (int band_idx = 0; band_idx < 5; ++band_idx)
                {
                    const auto band = static_cast<brainviz::data::FrequencyBand>(band_idx);
                    const auto& amplitudes = analyzer.get_band_amplitude(band, channel_to_analyze);

                    // avg amplitude
                    double sum = 0.0;
                    for (const auto& amp : amplitudes)
                    {
                        sum += amp;
                    }
                    double avg_amplitude = sum / amplitudes.size();

                    fmt::print("  {} band: average amplitude = {:.2f}\n",
                               frequency_band_to_string(band), avg_amplitude);
                }
            }
            catch (const std::exception& e)
            {
                fmt::print(stderr, "Error in frequency analysis: {}\n", e.what());
            }
        }

        fmt::print("\nDone\n");
    }
    catch (const std::exception& e)
    {
        fmt::print(stderr, "Unhandled exception: {}\n", e.what());
        return 1;
    }

    return 0;
}
