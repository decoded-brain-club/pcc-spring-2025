// visualization_utils.cpp
#include <algorithm>
#include <cmath>

#include <utils/visualization_utils.hpp>

namespace brainviz::visualization
{
    sf::VertexArray create_line_strip(const std::vector<sf::Vector2f>& points, const std::vector<int>& indices,
                                    const sf::Color color)
    {
        sf::VertexArray lineStrip(sf::PrimitiveType::LineStrip, indices.size());

        for (size_t i = 0; i < indices.size(); ++i)
        {
            if (int index = indices[i]; index >= 0 && index < points.size())
            {
                lineStrip[i].position = points[index];
                lineStrip[i].color = color;
            }
        }

        return lineStrip;
    }

    void closest_intersection(float& xPos, float& yPos, const double r)
    {
        const float divisor = std::sqrt(xPos * xPos + yPos * yPos);
        if (divisor < 0.0001f)
            return; // Avoid division by near-zero

        // Compute the two intersection points
        const float x1 = (static_cast<float>(r) * xPos) / divisor;
        const float y1 = (static_cast<float>(r) * yPos) / divisor;
        const float x2 = -x1;
        const float y2 = -y1;

        // Compute distances to the original point
        const auto d1 = static_cast<float>(std::sqrt(std::pow(x1 - xPos, 2) + std::pow(y1 - yPos, 2)));
        const auto d2 = static_cast<float>(std::sqrt(std::pow(x2 - xPos, 2) + std::pow(y2 - yPos, 2)));

        // Modify xPos and yPos to the closest intersection point
        if (d1 < d2)
        {
            xPos = x1;
            yPos = y1;
        }
        else
        {
            xPos = x2;
            yPos = y2;
        }
    }

    // TODO: this needs to be rethought, its very poorly implemented
    void enforce_aspect_ratio(sf::RenderWindow& window, const float aspectRatio)
    {
        sf::Vector2u size = window.getSize();

        const float newWidth = static_cast<float>(size.y) * aspectRatio;
        const float newHeight = static_cast<float>(size.x) / aspectRatio;

        if (static_cast<float>(size.x) / static_cast<float>(size.y) > aspectRatio)
        {
            window.setSize({static_cast<unsigned int>(newWidth), size.y});
        }
        else
        {
            window.setSize({size.x, static_cast<unsigned int>(newHeight)});
        }
    }
} // namespace brainviz::visualization