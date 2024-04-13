#pragma once
#include <SFML/Graphics.hpp>
#include "../settings.h"
#include "../world/world.h"
#include "../builder/builder.h"

#include "../Utils/time.h"
#include "../Utils/fps_manager.h"
#include "../Utils/fonts/font_renderer.hpp"
#include "../Utils/UI/Camera.hpp"
#include "../Utils/UI/line_graph.h"
#include "../Utils/NeuralNetworks/GeneticNeuralNetwork.h"
#include "../Utils/NeuralNetworks/NeuralNetworkUI.h"
#include "../Utils/UI/text_box.h"


class Simulation : SimulationSettings
{
	sf::RenderWindow m_window_ = sf::RenderWindow(sf::VideoMode(window_width, window_height), 
		simulation_name, sf::Style::None);

	FrameRateSmoothing<30> m_clock_{};
	Camera camera_{&m_window_, 0.25f};
	Builder m_builder_{&m_window_};

	// managing time
	StopWatch m_delta_time_{};
	float m_total_time_elapsed_ = 0.f;
	unsigned m_ticks_ = 0;

	Font m_title_font_{ &m_window_, 33, "src/Utils/fonts/Roboto-Bold.ttf" }; // larger m_text
	Font m_text_font_{ &m_window_, 17, "src/Utils/fonts/Roboto-Regular.ttf" };  // smaller m_text

	World m_world_{};

	// user controlled variables
	bool m_paused_ = false;
	bool m_debug_ = false;

	LineGraph<line_maximum_data, line_x_axis_increments> protozoa_population_graph_{ &m_window_, protozoa_graph_bounds };
	LineGraph<line_maximum_data, line_x_axis_increments> food_population_graph_{ &m_window_, food_graph_bounds };

	float test_data = 50.f;
	float test_data2 = 100.f;

	float test_data3 = 0;
	float test_data4 = 0;

	GeneticNeuralNetwork network{ 2, 1, 2 };
	NetworkRenderer net_renderer{ {200, 1750, 1800, 900}, &m_window_, &network };
	TextBox text_box{ text_renderer_bounds, &m_window_ };

	bool mouse_pressed_event = false;


public:
	Simulation();
	void run();

private:
	void update();
	void update_statistics();
	void render();

	// User Input
	void handle_events();
	void keyboard_input(const sf::Keyboard::Key& event_key_code);
	static void mouse_input();

	// more rendering
	void display_screen_info();
	void manage_frame_rate();
};