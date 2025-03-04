#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include <logging/logger.hpp>

int main()
{
	g_logger.set_level(LogLevel::Debug);

	g_logger.add_sink(FileSink("app.log"), //TODO: FIX THIS!! proper directory for logs
					  "[{timestamp:%Y-%m-%d %H:%M:%S}] [{level}] {message}",
					  LogLevel::Debug
	);

	auto window = sf::RenderWindow(sf::VideoMode({1920u, 1080u}), "CMake SFML Project");
	window.setFramerateLimit(144);
	if (!ImGui::SFML::Init(window))
		return -1;

	sf::Clock clock;
	while (window.isOpen())
	{
		while (const std::optional event = window.pollEvent())
		{
			ImGui::SFML::ProcessEvent(window, *event);

			if (event->is<sf::Event::Closed>())
			{
				window.close();
			}
		}

		ImGui::SFML::Update(window, clock.restart());

		// draw the imgui demo window
		ImGui::ShowDemoWindow();

		window.clear();
		ImGui::SFML::Render(window);
		window.display();
	}

	ImGui::SFML::Shutdown();
}
