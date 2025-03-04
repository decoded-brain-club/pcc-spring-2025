#pragma once

#include <vector>
#include <array>

#include <SFML/Graphics.hpp>

#include <electrode/electrode_set.hpp>

namespace brainviz::visualization
{
    /**
     * @brief Manages and renders electrode frame visualizations
     *
     * The ElectrodeFrameManager class provides functionality for drawing electrode
     * frames in different EEG systems (16, 32, 64, 128, or 256 channels). It handles
     * caching of vertex arrays for efficient rendering and maintains the frame data
     * that defines how electrodes should be connected.
     */
    class ElectrodeFrameManager
    {
    public:
        /**
         * @brief Get the singleton instance of ElectrodeFrameManager
         * @return Reference to the ElectrodeFrameManager instance
         */
        static ElectrodeFrameManager& instance();

        /**
         * @brief Get the frame data for a specific electrode system
         * @param system The electrode system type (16, 32, 64, 128, or 256 channels)
         * @return A reference to the frame data, where each inner vector represents a line connecting electrodes
         */
        const std::vector<std::vector<int>>& get_frame(electrode::SystemType system);

        /**
         * @brief Draw the electrode frame for a specific system
         * @param window The render window to draw on
         * @param system The electrode system type
         * @param points The vector of electrode positions in 2D space
         *
         * This method draws the electrode frame by connecting electrodes according to
         * the frame definition for the specified system. It uses cached vertex arrays
         * for improved performance.
         */
        void draw_frame(
           sf::RenderWindow& window,
           electrode::SystemType system,
           const std::vector<sf::Vector2f>& points
        );

    private:
        /**
         * @brief Constructor initializes the frame data for all electrode systems
         *
         * The constructor populates m_frameData with the connection information
         * for each supported electrode system.
         */
        ElectrodeFrameManager();

        static constexpr size_t NUM_SYSTEMS = 5; // System16, System32, System64, System128, System256

        /**
         * @brief Array of frame data for each electrode system
         *
         * Each outer vector represents a system, middle vectors represent lines,
         * and inner vectors contain electrode indices to connect.
         */
        std::array<std::vector<std::vector<int>>, NUM_SYSTEMS> m_frameData;

        /**
         * @brief Cached vertex arrays for efficient rendering
         *
         * These vertex arrays are updated when draw_frame is called and
         * reused to avoid recreating them on each frame.
         */
        std::array<std::vector<sf::VertexArray>, NUM_SYSTEMS> m_frameVertexArrays;
    };

    /**
     * @brief Draw frame connecting electrodes (optimized free function)
     * @param window The render window to draw on
     * @param system The electrode system type
     * @param points The vector of electrode positions in 2D space
     *
     * This is a convenience function that forwards the call to the
     * ElectrodeFrameManager instance's draw_frame method.
     */
    void draw_frame(
        sf::RenderWindow& window,
        electrode::SystemType system,
        const std::vector<sf::Vector2f>& points
    );

    /**
     * @brief Get frame definition for specific system
     * @param system The electrode system type
     * @return The frame data as a vector of vectors of electrode indices
     *
     * This is a convenience function that forwards the call to the
     * ElectrodeFrameManager instance's get_frame method.
     */
    std::vector<std::vector<int>> get_frame(electrode::SystemType system);

} // namespace brainviz::visualization