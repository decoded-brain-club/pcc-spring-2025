// visualization_utils.hpp
#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

namespace brainviz::visualization {

	// Create a line strip vertex array for SFML drawing
	sf::VertexArray create_line_strip(
		const std::vector<sf::Vector2f>& points,
		const std::vector<int>& indices,
		sf::Color color
	);

	// Check if a point is outside the head circle and adjust if needed
	void closest_intersection(float& xPos, float& yPos, double r);

	void enforce_aspect_ratio(sf::RenderWindow& window, float aspectRatio);

} // namespace brainviz::visualization