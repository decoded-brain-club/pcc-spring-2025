#include <memory>

#include <ui/electrode_renderer.hpp>
#include <utils/visualization_utils.hpp>
#include <fonts/roboto-regular.hpp>

namespace brainviz::visualization
{
    // ElectrodeVisualizationManager impl
    ElectrodeVisualizationManager& ElectrodeVisualizationManager::instance()
    {
        static ElectrodeVisualizationManager instance;
        return instance;
    }

    bool ElectrodeVisualizationManager::initialize()
    {
        if (!m_fontInitialized)
        {
            m_fontInitialized = m_font.openFromMemory(
                fonts::roboto_regular.data(), fonts::roboto_regular_size);
        }
        return m_fontInitialized;
    }

    void ElectrodeVisualizationManager::draw_electrodes(
        sf::RenderWindow& window,
        const electrode::ElectrodeSet& electrodeSet,
        std::vector<sf::Vector2f>& points,
        const double headRadius,
        const double circleRadius,
        const sf::Vector2u windowSize)
    {
        if (!m_fontInitialized)
            initialize();

        const bool recalcPositions =
                points.size() != electrodeSet.size() ||
                m_lastWindowSize != windowSize ||
                m_lastHeadRadius != headRadius ||
                m_lastCircleRadius != circleRadius;

        if (points.size() != electrodeSet.size())
            points.resize(electrodeSet.size());

        if (m_electrodeVisuals.size() < electrodeSet.size())
            m_electrodeVisuals.resize(electrodeSet.size());

        // means we need to recalculate positions
        if (recalcPositions)
        {
            for (const auto& electrode : electrodeSet)
            {
                auto xPos = static_cast<float>(electrode.getX(headRadius));
                auto yPos = static_cast<float>(electrode.getY(headRadius));

                // TODO: rethink this? the edge cases should probably be handled in the electrode system class
                if (electrode.name() == "P10" || electrode.name() == "Iz" || electrode.name() == "P9")
                {
                    closest_intersection(xPos, yPos, circleRadius);
                }

                xPos += static_cast<float>(windowSize.x) / 2.0f;
                yPos = static_cast<float>(windowSize.y) / 2.0f - yPos;

                points[electrode.id()] = sf::Vector2f(xPos, yPos);
            }

            m_lastWindowSize = windowSize;
            m_lastHeadRadius = headRadius;
            m_lastCircleRadius = circleRadius;
        }

        // draw
        for (const auto& electrode : electrodeSet)
        {
            const int id = electrode.id();

            // should? never happen but just in case initialization gets botched
            if (id < 0 || id >= static_cast<int>(points.size()))
                continue;

            const sf::Vector2f& position = points[id];

            // initialize the electroes once
            ElectrodeVisual& visual = m_electrodeVisuals[id];
            if (!visual.initialized)
            {
                visual.point.setRadius(3.0f);
                visual.point.setFillColor(sf::Color::White);
                visual.point.setOrigin({3.0f, 3.0f});

                visual.label = std::make_unique<sf::Text>(m_font);
                visual.label->setString(std::string(electrode.name()));
                visual.label->setFillColor(sf::Color::White);
                visual.label->setCharacterSize(15);

                visual.initialized = true;
            }

            visual.point.setPosition(position);
            if (visual.label)
            {
                visual.label->setPosition({position.x + 6.0f, position.y + 6.0f});
            }

            // draw
            window.draw(visual.point);
            if (visual.label)
            {
                window.draw(*visual.label);
            }
        }
    }

    void draw_electrodes(
        sf::RenderWindow& window,
        const electrode::ElectrodeSet& electrodeSet,
        std::vector<sf::Vector2f>& points,
        const double headRadius,
        const double circleRadius,
        const sf::Vector2u windowSize
    )
    {
        ElectrodeVisualizationManager::instance().draw_electrodes(
            window, electrodeSet, points, headRadius, circleRadius, windowSize);
    }
} // namespace brainviz::visualization
