#pragma once

#include <string_view>
#include <cmath>
#include <numbers>

#include <utils/compiler.hpp>

namespace brainviz::electrode
{
    class Electrode
    {
    public:
        constexpr Electrode() noexcept
            : m_id(-1), m_name(""), m_inclination(0.0), m_azimuth(0.0)
        {} // so array doesnt compain

        constexpr Electrode(const int id, const std::string_view name, const double inclination,
                            const double azimuth) noexcept
            : m_id(id), m_name(name), m_inclination(inclination), m_azimuth(azimuth)
        {
        }

        [[nodiscard]] constexpr int id() const noexcept
        {
            return m_id;
        }

        [[nodiscard]] constexpr std::string_view name() const noexcept
        {
            return m_name;
        }

        [[nodiscard]] constexpr double inclination() const noexcept
        {
            return m_inclination;
        }

        [[nodiscard]] constexpr double azimuth() const noexcept
        {
            return m_azimuth;
        }

        [[nodiscard]] constexpr double getX(const double radius = 100.0) const noexcept
        {
            // x = r sinθ cosφ
            return radius * sin(deg_to_rad(m_inclination)) * cos(deg_to_rad(m_azimuth));
        }

        [[nodiscard]] constexpr double getY(const double radius = 100.0) const noexcept
        {
            // y = r sinθ sinφ
            return radius * sin(deg_to_rad(m_inclination)) * sin(deg_to_rad(m_azimuth));
        }

        [[nodiscard]] constexpr double getZ(const double radius = 100.0) const noexcept
        {
            // z = r cosθ
            return radius * cos(deg_to_rad(m_inclination));
        }

        [[nodiscard]] constexpr bool operator==(const Electrode& other) const noexcept
        {
            return m_id == other.m_id && m_name == other.m_name;
        }

        [[nodiscard]] constexpr bool operator!=(const Electrode& other) const noexcept
        {
            return !(*this == other);
        }

    private:
        int m_id;
        std::string_view m_name;
        double m_inclination; // θ theta in degrees
        double m_azimuth; // phi in degrees

        FORCE_INLINE
        static constexpr double deg_to_rad(const double degrees) noexcept
        {
            return degrees * std::numbers::pi / 180.0;
        }
    };
}
