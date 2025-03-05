// main.cpp
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include <ui/electrode_visualization.hpp>  // Our integration with SFML
int main() {
    // Get desktop resolution and set aspect ratio
    const sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    const unsigned int screenWidth = desktopMode.size.x;
    const unsigned int screenHeight = desktopMode.size.y;
    const float aspectRatio = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);

    // Create a full-screen window
    sf::RenderWindow window(desktopMode, "The Bei Project");
    window.setFramerateLimit(144);
    const sf::Vector2u windowSize = window.getSize();

    // Head variables
    const double HEAD_RADIUS = (windowSize.y / 2.0) * 0.7;
    const double CIRCLE_RADIUS = HEAD_RADIUS * 1.2;

    // create the set
    brainviz::electrode::ElectrodeSet electrodeSet(brainviz::electrode::SystemType::System64);

    // Head (outer circle)
    sf::CircleShape head(static_cast<float>(CIRCLE_RADIUS));
    head.setFillColor({0, 0, 0, 0});
    head.setOutlineThickness(3);
    head.setOutlineColor({255, 255, 255});
    head.setPosition({
        static_cast<float>(windowSize.x / 2.0 - head.getRadius()),
        static_cast<float>(windowSize.y / 2.0 - head.getRadius())
    });

    // Plotting electrode data
    std::unordered_map<std::string, std::vector<double>> sampleData = {
        {"Fp1", {0.5, 1.2, -0.8, 3.4, -2.1}},
        {"Fp2", {1.1, 0.7, -1.5, 2.3, -3.0}},
        {"O1", {-0.4, 2.0, 1.7, -0.6, 3.1}},
        {"O2", {2.2, -1.1, 0.4, 1.5, -0.9}}
    };

    std::vector<std::pair<sf::Vector2f, std::vector<double>>> dereferencedData;
    int moment = 0;

    // Prepare data visualization points
    std::vector<sf::Vector2f> points;

    // precompute the electrode positions for the current window size, this is done once and updated when the window is resized
    brainviz::visualization::draw_electrodes(
        window, electrodeSet, points, HEAD_RADIUS, CIRCLE_RADIUS, windowSize
    );

    // Process sample data
    for (const auto& [channel_name, values] : sampleData) {
        // Find electrode by name
        auto electrode_result = electrodeSet.find(channel_name);

        if (electrode_result) {
            const auto& electrode = electrode_result.value();
            int id = electrode.id();

            if (id >= 0 && id < points.size()) {
                const sf::Vector2f& position = points[id];
                dereferencedData.emplace_back(position, values);
            }
        }
    }

    while (window.isOpen()) {
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            if (event->is<sf::Event::Resized>()) {
                brainviz::visualization::enforce_aspect_ratio(window, aspectRatio);
            }
        }

        window.clear();
        window.draw(head);

        // draw the electrodes
        brainviz::visualization::draw_electrodes(
            window, electrodeSet, points, HEAD_RADIUS, CIRCLE_RADIUS, windowSize
        );

        // draw the lines
        brainviz::visualization::draw_frame(window, brainviz::electrode::SystemType::System64, points);

        for (const auto& [position, values] : dereferencedData) {
            if (!values.empty()) {
                const auto intensity = static_cast<float>(values[moment % values.size()]);
                const float radius = std::max(2.0f, 10.0f + intensity * 2.0f); // Min size 2, max influenced by intensity

                // Draw circle
                sf::CircleShape amp(radius);
                amp.setFillColor(sf::Color::Red);
                amp.setPosition({position.x - radius, position.y - radius});
                window.draw(amp);
            }
        }

        if (!dereferencedData.empty() && !dereferencedData.front().second.empty()) {
            moment = (moment + 1) % static_cast<int>(dereferencedData.front().second.size());
        }

        window.display();
    }
    return 0;
}