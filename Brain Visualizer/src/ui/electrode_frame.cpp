// electrode_frame.cpp
#include <cassert>

#include <ui/electrode_frame.hpp>

namespace brainviz::visualization
{
    ElectrodeFrameManager& ElectrodeFrameManager::instance()
    {
        static ElectrodeFrameManager instance;
        return instance;
    }

    ElectrodeFrameManager::ElectrodeFrameManager()
    {
        // System64 frame data
        // TODO: implement the actual frame data for the rest of the systems
        m_frameData[static_cast<size_t>(electrode::SystemType::System64)] = {
            {23, 22, 21, 20, 19, 30, 56, 57, 58, 59, 60}, // P-line
            {26, 28, 63}, // O-line
            {24, 25, 29, 62, 61}, // PO-line
            {16, 17, 18, 31, 55, 54, 53}, // CP-line
            {14, 13, 12, 11, 47, 48, 49, 50, 51}, // C-line
            {8, 9, 10, 46, 45, 44, 43}, // FC-line
            {6, 5, 4, 3, 37, 38, 39, 40, 41}, // F-line
            {1, 2, 36, 35, 34}, // AF-line
            {0, 32, 33}, // FP-line
            {32, 3, 10, 11, 18, 19, 28}, // Vertical -1
            {32, 38, 45, 48, 55, 56, 28}, // Vertical -2
            {2, 4, 9, 12, 17, 20, 25}, // Vertical -3
            {35, 39, 44, 49, 54, 57, 62}, // Vertical -4
            {5, 8, 13, 16, 21}, // Vertical -5
            {40, 43, 50, 53, 58}, // Vertical -6
            {1, 6, 7, 14, 15, 22, 24}, // Vertical -7
            {34, 41, 42, 51, 52, 59, 61}, // Vertical -8
            {32, 36, 37, 46, 47, 31, 30, 29, 28, 27} // Vertical -z
        };

        m_frameData[static_cast<size_t>(electrode::SystemType::System16)] = {
        };

        m_frameData[static_cast<size_t>(electrode::SystemType::System32)] = {
        };

        // ...
    }

    const std::vector<std::vector<int>>& ElectrodeFrameManager::get_frame(const electrode::SystemType system)
    {
        const auto index = static_cast<size_t>(system);
        assert(index < NUM_SYSTEMS && "Invalid electrode system type");
        return m_frameData[index];
    }

    void ElectrodeFrameManager::draw_frame(
        sf::RenderWindow& window,
        const electrode::SystemType system,
        const std::vector<sf::Vector2f>& points)
    {
        const auto systemIndex = static_cast<size_t>(system);
        assert(systemIndex < NUM_SYSTEMS && "Invalid electrode system type");

        const auto& frameData = m_frameData[systemIndex];

        // get the cached vertex arrays for the current system or create them if they don't exist
        auto& vertexArrayCache = m_frameVertexArrays[systemIndex];
        if (vertexArrayCache.size() != frameData.size())
        {
            vertexArrayCache.resize(frameData.size());
            for (size_t i = 0; i < frameData.size(); ++i)
            {
                vertexArrayCache[i] = sf::VertexArray(sf::PrimitiveType::LineStrip, frameData[i].size());
            }
        }

        for (size_t i = 0; i < frameData.size(); ++i)
        {
            const auto& indices = frameData[i];
            auto& vertices = vertexArrayCache[i];

            for (size_t j = 0; j < indices.size(); ++j)
            {
                if (const int index = indices[j]; index >= 0 && index < static_cast<int>(points.size()))
                {
                    vertices[j].position = points[index];
                    vertices[j].color = sf::Color::Green;
                }
            }
        }

        for (const auto& vertices : vertexArrayCache)
        {
            window.draw(vertices);
        }
    }

    // optimized draw frame
    void draw_frame(
        sf::RenderWindow& window,
        const electrode::SystemType system,
        const std::vector<sf::Vector2f>& points
    )
    {
        ElectrodeFrameManager::instance().draw_frame(window, system, points);
    }

    std::vector<std::vector<int>> get_frame(const electrode::SystemType system)
    {
        return ElectrodeFrameManager::instance().get_frame(system);
    }
} // namespace brainviz::visualization