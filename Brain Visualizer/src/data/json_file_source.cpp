#include <iostream>
#include <system_error>

#include <fmt/format.h>

// TODO: CHANGE " to <
#include <expected>
#include <data/json_file_source.hpp>

namespace brainviz
{
    namespace data
    {
        JSONFileSource::JSONFileSource(const std::string_view filePath)
            : m_filePath(filePath.data()), m_fileOpen(false)
        {
        }

        JSONFileSource::~JSONFileSource()
        {
            JSONFileSource::close();
        }

        bool JSONFileSource::is_data_available() const
        {
            std::ifstream file(m_filePath);
            return file.good();
        }

        std::expected<bool, JSONFileSource::validation_error> JSONFileSource::validate_json_structure(const simdjson::dom::element& root)
        {
            if (!root.is_object())
            {
                return false;
            }

            bool has_channels = false;

            for (auto field : root.get_object())
            {
                if (!field.value.is_array())
                {
                    return std::unexpected(validation_error::FIELD_NOT_ARRAY);
                }

                has_channels = true;

                simdjson::dom::array array;
                auto error = field.value.get_array().get(array);
                if (error || array.size() == 0)
                {
                    return std::unexpected(validation_error::EMPTY_ARRAY);
                }

                auto first_value = array.at(0);
                if (!first_value.is_double())
                {
                    return std::unexpected(validation_error::VALUE_NOT_DOUBLE);
                }
            }

            return has_channels;
        }

        std::unique_ptr<EEGData> JSONFileSource::load_data()
        {
            if (!is_open() && !open())
            {
                throw std::system_error(errno, std::system_category(),
                                        fmt::format("Failed to open file: {}", m_filePath));
            }

            auto eegData = std::make_unique<EEGData>();

            try
            {
                // Parse the JSON file
                simdjson::dom::element root;
                auto error = m_parser_.load(m_filePath).get(root);

                if (error)
                {
                    throw std::runtime_error(fmt::format("Failed to parse JSON: {}", simdjson::error_message(error)));
                }

                if (!validate_json_structure(root))
                {
                    throw std::runtime_error("Invalid JSON structure for EEG data");
                }

                for (auto& field : root.get_object())
                {
                    auto channelName = std::string_view(field.key);
                    std::vector<double> values;

                    simdjson::dom::array array;
                    error = field.value.get_array().get(array);

                    if (error)
                    {
                        throw std::runtime_error(fmt::format("Channel {} does not contain an array of values",
                                                             channelName));
                    }

                    values.reserve(array.size());

                    // Extract each value
                    for (auto value : array)
                    {
                        double doubleValue;
                        error = value.get_double().get(doubleValue);

                        if (error)
                        {
                            // TODO: then it might be an integer, will this ever be the case?
                            int64_t intValue;
                            error = value.get_int64().get(intValue);

                            if (error)
                            {
                                throw std::runtime_error(fmt::format("Non-numeric value in channel {}", channelName));
                            }

                            doubleValue = static_cast<double>(intValue);
                        }

                        values.push_back(doubleValue);
                    }

                    eegData->set_channel(channelName, std::move(values));
                }

                // TODO: fow now this is what we set it too, realistically we should get this from the backend
                eegData->m_samplingRate = 250.0;

                return eegData;
            }
            catch (const std::exception& e)
            {
                throw std::runtime_error(fmt::format("Error loading EEG data: {}", e.what()));
            }
        }

        bool JSONFileSource::open()
        {
            if (!is_data_available())
            {
                return false;
            }

            m_fileOpen = true;
            return true;
        }

        void JSONFileSource::close()
        {
            m_fileOpen = false;
        }

        bool JSONFileSource::is_open() const
        {
            return m_fileOpen;
        }

        std::string JSONFileSource::get_source_name() const
        {
            return "JSON File: " + m_filePath;
        }

        void JSONFileSource::set_file_path(const std::string_view newFilePath)
        {
            if (is_open())
            {
                close();
            }
            m_filePath = newFilePath;
        }

        const std::string& JSONFileSource::get_file_path() const
        {
            return m_filePath;
        }
    } // namespace data
} // namespace brainviz
