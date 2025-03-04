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

// multithreading imports, rendering and updating happens on two separate threads, with rendering on the main thread
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>

inline static constexpr int frame_smoothing = 30;
inline static constexpr float dt = 1.0f / 60.0f; // 30 updates per second

class Simulation : SimulationSettings, UI_Settings, TextSettings
{
	sf::RenderWindow m_window_ = sf::RenderWindow(sf::VideoMode(window_width, window_height), 
		simulation_name, sf::Style::None);

	FrameRateSmoothing<frame_smoothing> m_clock_{};
	Camera camera_{&m_window_, 0.25f};
	Builder m_builder_{&m_window_};

	// managing time
	StopWatch m_delta_time_{};
	float m_total_time_elapsed_ = 0.f;
	unsigned m_ticks_ = 0;

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

	// A shared game state mutex
	std::mutex gameStateMutex;
	std::atomic<bool> running{true};

	// statistics
	float fps_;


public:
	Simulation();
	void init_line_graphs();
	void init_text_box();
	void init_network_renderer();
	void run();
	void render_loop();
	void update_loop();

private:
	void update_one_frame();
	void update_test_data();
	void update_line_graphs();
	void draw_everything();
	void render();

	// User Input
	void handle_events();
	void keyboard_input(const sf::Keyboard::Key& event_key_code);
	static void mouse_input();

	// more rendering
	void manage_frame_rate();
};