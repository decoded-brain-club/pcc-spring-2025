#include <imgui-SFML.h>
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <iostream>

#include <ui/electrode_visualization.hpp>
#include <data/interface.hpp>
#include <data/json_file_source.hpp>
#include <analysis/batch_analyzer.hpp>
#include <electrode/electrode_set.hpp>

#include <ui/frequency_band_selector.hpp>
#include <ui/detail/electrode_state_manager.hpp>


using namespace brainviz;

// TODO: move this to a separate file
class EEGVisualizer
{
public:
    explicit EEGVisualizer(const ElectrodeStateManager& stateManager)
        : m_stateManager(stateManager)
    {
    }

    void render(sf::RenderWindow& window, const std::vector<sf::Vector2f>& electrodePoints) const
    {
        if (m_stateManager.is_single_band_mode())
        {
            render_single_band(window, electrodePoints);
        }
        else
        {
            render_all_bands(window, electrodePoints);
        }
    }

private:
    const ElectrodeStateManager& m_stateManager;

    // TODO: make this return raw rgba values instead of sf::Color then conv to whatever
    static sf::Color get_band_color(const data::FrequencyBand band)
    {
        switch (band)
        {
            case data::FrequencyBand::Delta:
                return {170, 223, 237};
            case data::FrequencyBand::Theta:
                return {207, 230, 184};
            case data::FrequencyBand::Alpha:
                return {251, 247, 186};
            case data::FrequencyBand::Beta:
                return {255, 214, 166};
            case data::FrequencyBand::Gamma:
                return {248, 172, 174};
            default:
                return {255, 255, 255};
        }
    }

    void render_single_band(sf::RenderWindow& window, const std::vector<sf::Vector2f>& electrodePoints) const
    {
        const brainviz::data::FrequencyBand selectedBand = m_stateManager.get_selected_band();
        const int bandIndex = static_cast<int>(selectedBand);
        const auto& visualizationData = m_stateManager.get_visualization_data();

        const sf::Color color = get_band_color(selectedBand);

        static constexpr float baseRadius = 30.0f;

        for (const auto& [id, data] : visualizationData)
        {
            if (id < 0 || id >= static_cast<int>(electrodePoints.size()))
                continue;

            const sf::Vector2f& position = electrodePoints[id];

            float radius = baseRadius * data.radii[bandIndex];

            sf::Color electrodeColor = color;
            electrodeColor.a = static_cast<uint8_t>(255 * data.alphas[bandIndex]);

            sf::CircleShape circle(radius);
            circle.setOrigin({radius, radius});
            circle.setPosition(position);
            circle.setFillColor(electrodeColor);
            window.draw(circle);
        }
    }

    void render_all_bands(sf::RenderWindow& window, const std::vector<sf::Vector2f>& electrodePoints) const
    {
        const brainviz::data::FrequencyBand selectedBand = m_stateManager.get_selected_band();
        const auto& visualizationData = m_stateManager.get_visualization_data();

        for (const auto& [id, data] : visualizationData)
        {
            if (id < 0 || id >= static_cast<int>(electrodePoints.size()))
                continue;

            const sf::Vector2f& position = electrodePoints[id];

            for (int i = 0; i < 5; ++i)
            {
                const auto band = static_cast<brainviz::data::FrequencyBand>(i);

                const float baseRadius = 6.0f + i * 3.0f;
                float radius = baseRadius * data.radii[i];

                sf::Color color = get_band_color(band);
                color.a = static_cast<uint8_t>(255 * data.alphas[i]);

                if (band == selectedBand)
                {
                    color.a = 255;
                }

                sf::CircleShape circle(radius);
                circle.setOrigin({radius, radius});
                circle.setPosition(position);
                circle.setFillColor(color);
                window.draw(circle);
            }
        }
    }
};

int main()
{
    // Get desktop resolution and set aspect ratio
    const sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    const unsigned int screenWidth = desktopMode.size.x;
    const unsigned int screenHeight = desktopMode.size.y;
    const float aspectRatio = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);

    // Create a full-screen window
    sf::RenderWindow window(desktopMode, "BrainViz EEG Visualizer",
                            sf::Style::Default,
                            sf::State::Windowed,
                            sf::ContextSettings(0, 0, 8));

    window.setFramerateLimit(144);
    const sf::Vector2u windowSize = window.getSize();

    ImGui::SFML::Init(window);

    // Head variables
    const double HEAD_RADIUS = (windowSize.y / 2.0) * 0.7;
    const double CIRCLE_RADIUS = HEAD_RADIUS * 1.2;

    // Create the 64-channel electrode set
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

    std::cout << "Loading EEG data from JSON file..." << std::endl;

    auto dataSource = std::make_unique<brainviz::data::JSONFileSource>("data.json");
    if (!dataSource->open())
    {
        std::cerr << "Error: Failed to open data.json file" << std::endl;
        return 1;
    }

    auto eegData = dataSource->load_data();
    if (!eegData)
    {
        std::cerr << "Error: Failed to load data from data.json" << std::endl;
        return 1;
    }

    std::cout << "JSON data loaded successfully" << std::endl;

    std::cout << "Processing EEG data..." << std::endl;
    brainviz::analysis::BatchAnalyzer analyzer(*eegData, 256, 75.0);
    analyzer.process_all_channels();
    std::cout << "Data processing complete" << std::endl;

    FrequencyBandSelector bandSelector;

    ElectrodeStateManager stateManager(electrodeSet, analyzer);

    EEGVisualizer visualizer(stateManager);

    std::vector<sf::Vector2f> points;

    brainviz::visualization::draw_electrodes(
        window, electrodeSet, points, HEAD_RADIUS, CIRCLE_RADIUS, windowSize
    );

    sf::Clock deltaClock;

    while (window.isOpen())
    {
        // Process events
        while (const std::optional<sf::Event> event = window.pollEvent())
        {
            ImGui::SFML::ProcessEvent(window, *event);

            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            if (event->is<sf::Event::Resized>())
            {
                brainviz::visualization::enforce_aspect_ratio(window, aspectRatio);
            }
        }

        const sf::Time dt = deltaClock.restart();
        ImGui::SFML::Update(window, dt);

        const float deltaTimeSeconds = dt.asSeconds();

        stateManager.update(deltaTimeSeconds, bandSelector.get_animation_speed());

        bandSelector.set_frame_info(stateManager.get_frame_index(), stateManager.get_max_frame_index());

        bandSelector.render();


        window.clear({20, 20, 30});

        window.draw(head);

        brainviz::visualization::draw_electrodes(
            window, electrodeSet, points, HEAD_RADIUS, CIRCLE_RADIUS, windowSize
        );
        brainviz::visualization::draw_frame(window, brainviz::electrode::SystemType::System64, points);

        visualizer.render(window, points);

        ImGui::SFML::Render(window);

        window.display();
    }

    ImGui::SFML::Shutdown();

    return 0;
}
