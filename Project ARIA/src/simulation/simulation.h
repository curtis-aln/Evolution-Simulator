#pragma once
#include <SFML/Graphics.hpp>
#include "../settings.h"
#include "../world/world.h"
#include "../builder/builder.h"

#include "../Utils/time.h"
#include "../Utils/fps_manager.h"
#include "../Utils/font_renderer.hpp"
#include "../Utils/Camera.hpp"
#include "../Utils/line_graph.h"


class Simulation : SimulationSettings
{
	sf::RenderWindow m_window_ = sf::RenderWindow(sf::VideoMode(window_width, window_height), 
		simulation_name, sf::Style::None);

	FrameRateSmoothing<30> m_clock_{};
	Camera camera{&m_window_, 1.0f};
	Builder m_builder_{&m_window_};

	// managing time
	StopWatch m_delta_time_{};
	float m_total_time_elapsed_ = 0.f;
	unsigned m_ticks_ = 0;

	Font m_title_font_{ &m_window_, 33, "src/Utils/fonts/Roboto-Bold.ttf" }; // larger text
	Font m_text_font_{ &m_window_, 17, "src/Utils/fonts/Roboto-Regular.ttf" };  // smaller text

	World m_world_{};

	// user controlled variables
	bool m_paused_ = false;
	bool m_debug_ = false;

	LineGraph protozoa_population_graph{ &m_window_, {1100, 200, 700, 200} };
	


public:
	Simulation();
	void run();

private:
	void update();
	void render();

	// User Input
	void handle_events();
	void keyboard_input(const sf::Keyboard::Key& event_key_code);
	void mouse_input();

	// more rendering
	void display_screen_info();
	void manage_frame_rate();
};