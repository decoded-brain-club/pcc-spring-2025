#pragma once

#include <memory>
#include <vector>

#include <SFML/Graphics.hpp>

#include <electrode/electrode_set.hpp>

namespace brainviz::visualization {

    /**
     * @brief Manages and renders electrode visualizations
     *
     * The ElectrodeVisualizationManager class provides functionality for drawing electrodes
     * from different EEG systems. It handles the creation and caching of visual elements
     * for efficient rendering and manages the conversion of electrode coordinates to
     * screen space.
     */
    class ElectrodeVisualizationManager
    {
    public:
        /**
         * @brief Get the singleton instance of ElectrodeVisualizationManager
         * @return Reference to the ElectrodeVisualizationManager instance
         */
        static ElectrodeVisualizationManager& instance();

        /**
         * @brief Initialize the visualization manager
         * @return True if initialization was successful, false otherwise
         *
         * This method loads the required font for electrode labels.
         * It should be called before any drawing takes place.
         */
        bool initialize();

        /**
         * @brief Draw electrodes from an electrode set
         * @param window The render window to draw on
         * @param electrodeSet The set of electrodes to visualize
         * @param points Reference to a vector that will store the 2D positions of electrodes
         * @param headRadius The radius of the head model in pixels
         * @param circleRadius The radius of the circle constraint for electrode placement
         * @param windowSize The current window dimensions
         *
         * This method calculates electrode positions if necessary, creates and caches
         * visual elements, and draws both electrode points and their labels. The positions
         * are stored in the points vector for potential reuse in other visualizations.
         */
        void draw_electrodes(
           sf::RenderWindow& window,
           const electrode::ElectrodeSet& electrodeSet,
           std::vector<sf::Vector2f>& points,
           double headRadius,
           double circleRadius,
           sf::Vector2u windowSize
        );

    private:
        /**
         * @brief Default constructor
         *
         * The constructor is private to enforce the singleton pattern.
         */
        ElectrodeVisualizationManager() = default;

        /**
         * @brief Font used for electrode labels
         */
        sf::Font m_font;

        /**
         * @brief Flag indicating if the font has been successfully loaded
         */
        bool m_fontInitialized = false;

        /**
         * @brief Last window size used for electrode position calculation
         *
         * Used to determine if positions need to be recalculated due to window resizing.
         */
        sf::Vector2u m_lastWindowSize;

        /**
         * @brief Last head radius used for electrode position calculation
         */
        double m_lastHeadRadius = 0.0;

        /**
         * @brief Last circle radius used for electrode position calculation
         */
        double m_lastCircleRadius = 0.0;

        /**
         * @brief Structure containing visual elements for a single electrode
         *
         * This structure caches the visual elements for each electrode to
         * avoid recreating them on each frame.
         */
        struct ElectrodeVisual {
           /**
            * @brief Circle shape representing the electrode point
            */
           sf::CircleShape point;

           /**
            * @brief Text label showing the electrode name
            */
           std::unique_ptr<sf::Text> label;

           /**
            * @brief Flag indicating if the visual elements have been initialized
            */
           bool initialized = false;

           /**
            * @brief Default constructor
            */
           ElectrodeVisual() : initialized(false) {}
        };

        /**
         * @brief Vector of cached visual elements for all electrodes
         */
        std::vector<ElectrodeVisual> m_electrodeVisuals;
    };

    /**
     * @brief Draw electrodes using the optimized visualization system
     * @param window The render window to draw on
     * @param electrodeSet The set of electrodes to visualize
     * @param points Reference to a vector that will store the 2D positions of electrodes
     * @param headRadius The radius of the head model in pixels
     * @param circleRadius The radius of the circle constraint for electrode placement
     * @param windowSize The current window dimensions
     *
     * This is a convenience function that forwards the call to the
     * ElectrodeVisualizationManager instance's draw_electrodes method.
     */
    void draw_electrodes(
       sf::RenderWindow& window,
       const electrode::ElectrodeSet& electrodeSet,
       std::vector<sf::Vector2f>& points,
       double headRadius,
       double circleRadius,
       sf::Vector2u windowSize
    );

} // namespace brainviz::visualization