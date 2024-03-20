#pragma once
#include <SFML/Graphics.hpp>
#include "../settings.h"
#include "../world/world.h"

#include "../Utils/time.h"
#include "../Utils/fps_manager.h"
#include "../Utils/font_renderer.hpp"


class Simulation : SimulationSettings
{
	// SFML window and clock for managing frame rates
	sf::RenderWindow m_window_ = sf::RenderWindow(sf::VideoMode(window_width, window_height), 
		simulation_name, sf::Style::None);
	sf::Clock m_clock_{};

	BetterFrameRates<30> m_frameRateManager{};

	DeltaTime deltaTime{};
	float m_total_time_elapsed = 0.f;
	unsigned m_ticks = 0;

	FontManager title_font{ &m_window_, 35, "src/Utils/fonts/Roboto-Bold.ttf" }; // larger text
	FontManager text_font{ &m_window_, 25, "src/Utils/fonts/Roboto-Regular.ttf" };  // smaller text

	World m_World{};

	// user controlled variables
	bool m_paused_ = false;
	bool m_debug_ = false;
	


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
	void display_frame_rate();
	void display_statistics();
	void manage_framerate();

	// more updating
};