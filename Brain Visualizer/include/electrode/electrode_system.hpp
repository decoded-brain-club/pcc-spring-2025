#pragma once

#include <array>
#include <span>
#include <string>
#include <string_view>
#include <expected>

#include <fmt/format.h>

#include <electrode/electrode.hpp>

namespace brainviz::electrode
{
    enum class SystemType
    {
        System16,
        System32,
        System64,
        System128,
        System256
    };

    class ElectrodeSystem
    {
    public:
        /**
         * @brief Get an electrode system by type
         * @param type The system type (16, 32, 64, 128, or 256 channels)
         * @return A span of electrodes for the requested system
         */
        [[nodiscard]] static std::span<const Electrode> get(const SystemType type)
        {
            initialize();

            switch (type)
            {
                case SystemType::System16:
                    return std::span(s_system16);
                case SystemType::System32:
                    return std::span(s_system32);
                case SystemType::System64:
                    return std::span(s_system64);
                case SystemType::System128:
                    return std::span(s_system128);
                case SystemType::System256:
                    return std::span(s_system256);
                default:
                    throw std::invalid_argument("Unknown electrode system type");
            }
        }

        /**
         * @brief Find an electrode by name within a system
         * @param type The system type
         * @param name The electrode name to find
         * @return The electrode if found, otherwise std::nullopt
         */
        [[nodiscard]] static std::expected<Electrode, std::string> find_by_name(
            const SystemType type, std::string_view name)
        {
            auto electrodes = get(type);

            const auto it = std::ranges::find_if(electrodes,
                                                 [name] (const Electrode& e) {
                                                     return e.name() == name;
                                                 });

            if (it != electrodes.end())
            {
                return *it;
            }

            return std::unexpected(fmt::format("Electrode '{}' not found in the {} system",
                                               name, system_type_to_string(type)));
        }

        /**
         * @brief Find an electrode by ID within a system
         * @param type The system type
         * @param id The electrode ID to find
         * @return The electrode if found, otherwise std::nullopt
         */
        [[nodiscard]] static std::expected<Electrode, std::string> find_by_id(
            const SystemType type, int id)
        {
            auto electrodes = get(type);

            const auto it = std::ranges::find_if(electrodes,
                                           [id] (const Electrode& e) {
                                               return e.id() == id;
                                           });

            if (it != electrodes.end())
            {
                return *it;
            }

            return std::unexpected(fmt::format("Electrode with ID {} not found in the {} system",
                                               id, system_type_to_string(type)));
        }

        /**
         * @brief Get the number of electrodes in a system
         * @param type The system type
         * @return The number of electrodes
         */
        [[nodiscard]] static constexpr size_t size(const SystemType type) noexcept
        {
            switch (type)
            {
                case SystemType::System16:
                    return 16;
                case SystemType::System32:
                    return 32;
                case SystemType::System64:
                    return 64;
                case SystemType::System128:
                    return 128;
                case SystemType::System256:
                    return 256;
                default:
                    return 0;
            }
        }

        /**
         * @brief Convert system type to string representation
         * @param type The system type
         * @return String representation
         */
        [[nodiscard]] static constexpr std::string_view system_type_to_string(const SystemType type) noexcept
        {
            switch (type)
            {
                case SystemType::System16:
                    return "16-channel";
                case SystemType::System32:
                    return "32-channel";
                case SystemType::System64:
                    return "64-channel";
                case SystemType::System128:
                    return "128-channel";
                case SystemType::System256:
                    return "256-channel";
                default:
                    return "Unknown";
            }
        }

    private:
        static void initialize()
        {
            static bool initialized = false;
            if (initialized)
                return;

            init_system16();
            init_system32();
            init_system64();
            init_system128();
            init_system256();

            initialized = true;
        }

        static inline std::array<Electrode, 16> s_system16;
        static inline std::array<Electrode, 32> s_system32;
        static inline std::array<Electrode, 64> s_system64;
        static inline std::array<Electrode, 128> s_system128;
        static inline std::array<Electrode, 256> s_system256;

        static void init_system16()
        {
            s_system16 = {
                Electrode(0, "Fp1", -92, -72),
                Electrode(1, "Fp2", 92, 72),
                Electrode(2, "F4", 60, 51),
                Electrode(3, "Fz", 46, 90),
                Electrode(4, "F3", -60, -51),
                Electrode(5, "T7", -92, 0),
                Electrode(6, "C3", -46, 0),
                Electrode(7, "Cz", 0, 0),
                Electrode(8, "C4", 46, 0),
                Electrode(9, "T8", 92, 0),
                Electrode(10, "P4", 60, -51),
                Electrode(11, "Pz", 46, -90),
                Electrode(12, "P3", -60, 51),
                Electrode(13, "O1", -92, 72),
                Electrode(14, "Oz", 92, -90),
                Electrode(15, "O2", 92, -72)
            };
        }

        static void init_system32()
        {
            s_system32 = {
                Electrode(0, "Fp1", -92, -72),
                Electrode(1, "AF3", -74, -65),
                Electrode(2, "F7", -92, -36),
                Electrode(3, "F3", -60, -51),
                Electrode(4, "FC1", -32, -45),
                Electrode(5, "FC5", -72, -21),
                Electrode(6, "T7", -92, 0),
                Electrode(7, "C3", -46, 0),
                Electrode(8, "CP1", -32, 45),
                Electrode(9, "CP5", -72, 21),
                Electrode(10, "P7", -92, 36),
                Electrode(11, "P3", -60, 51),
                Electrode(12, "Pz", 46, -90),
                Electrode(13, "PO3", -74, 65),
                Electrode(14, "O1", -92, 72),
                Electrode(15, "Oz", 92, -90),
                Electrode(16, "O2", 92, -72),
                Electrode(17, "PO4", 74, -65),
                Electrode(18, "P4", 60, -51),
                Electrode(19, "P8", 92, -36),
                Electrode(20, "CP6", 72, -21),
                Electrode(21, "CP2", 32, -45),
                Electrode(22, "C4", 46, 0),
                Electrode(23, "T8", 92, 0),
                Electrode(24, "FC6", 72, 21),
                Electrode(25, "FC2", 32, 45),
                Electrode(26, "F4", 60, 51),
                Electrode(27, "F8", 92, 36),
                Electrode(28, "AF4", 74, 65),
                Electrode(29, "Fp2", 92, 72),
                Electrode(30, "Fz", 46, 90),
                Electrode(31, "Cz", 0, 0)
            };
        }

        static void init_system64()
        {
            s_system64 = {
                Electrode(0, "Fp1", -92, -72),
                Electrode(1, "AF7", -92, -54),
                Electrode(2, "AF3", -74, -65),
                Electrode(3, "F1", -50, -68),
                Electrode(4, "F3", -60, -51),
                Electrode(5, "F5", -75, -41),
                Electrode(6, "F7", -92, -36),
                Electrode(7, "FT7", -92, -18),
                Electrode(8, "FC5", -72, -21),
                Electrode(9, "FC3", -50, -28),
                Electrode(10, "FC1", -32, -45),
                Electrode(11, "C1", -23, 0),
                Electrode(12, "C3", -46, 0),
                Electrode(13, "C5", -69, 0),
                Electrode(14, "T7", -92, 0),
                Electrode(15, "TP7", -92, 18),
                Electrode(16, "CP5", -72, 21),
                Electrode(17, "CP3", -50, 28),
                Electrode(18, "CP1", -32, 45),
                Electrode(19, "P1", -50, 68),
                Electrode(20, "P3", -60, 51),
                Electrode(21, "P5", -75, 41),
                Electrode(22, "P7", -92, 36),
                Electrode(23, "P9", -115, 36),
                Electrode(24, "PO7", -92, 54),
                Electrode(25, "PO3", -74, 65),
                Electrode(26, "O1", -92, 72),
                Electrode(27, "Iz", 115, -90),
                Electrode(28, "Oz", 92, -90),
                Electrode(29, "POz", 69, -90),
                Electrode(30, "Pz", 46, -90),
                Electrode(31, "CPz", 23, -90),
                Electrode(32, "Fpz", 92, 90),
                Electrode(33, "Fp2", 92, 72),
                Electrode(34, "AF8", 92, 54),
                Electrode(35, "AF4", 74, 65),
                Electrode(36, "AFz", 69, 90),
                Electrode(37, "Fz", 46, 90),
                Electrode(38, "F2", 50, 68),
                Electrode(39, "F4", 60, 51),
                Electrode(40, "F6", 75, 41),
                Electrode(41, "F8", 92, 36),
                Electrode(42, "FT8", 92, 18),
                Electrode(43, "FC6", 72, 21),
                Electrode(44, "FC4", 50, 28),
                Electrode(45, "FC2", 32, 45),
                Electrode(46, "FCz", 23, 90),
                Electrode(47, "Cz", 0, 0),
                Electrode(48, "C2", 23, 0),
                Electrode(49, "C4", 46, 0),
                Electrode(50, "C6", 69, 0),
                Electrode(51, "T8", 92, 0),
                Electrode(52, "TP8", 92, -18),
                Electrode(53, "CP6", 72, -21),
                Electrode(54, "CP4", 50, -28),
                Electrode(55, "CP2", 32, -45),
                Electrode(56, "P2", 50, -68),
                Electrode(57, "P4", 60, -51),
                Electrode(58, "P6", 75, -41),
                Electrode(59, "P8", 92, -36),
                Electrode(60, "P10", 115, -36),
                Electrode(61, "PO8", 92, -54),
                Electrode(62, "PO4", 74, -65),
                Electrode(63, "O2", 92, -72)
            };
        }

        static void init_system128()
        {
            // TODO: add the rest later
            s_system128[0] = Electrode(0, "A1", 0, 0); // Cz
            s_system128[1] = Electrode(1, "A2", 11.5, -90);
            s_system128[2] = Electrode(2, "A3", 23, -90); // CPz
        }

        static void init_system256()
        {
            // TODO: add the rest later
            s_system256[0] = Electrode(0, "A1", 0, 0); // Cz
            s_system256[1] = Electrode(1, "A2", 9, -90);
            s_system256[2] = Electrode(2, "A3", 18, -90);
        }
    };
}
