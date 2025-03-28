#pragma once
#include <SFML/Graphics.hpp>
#include "../settings.h"
#include "../world/world.h"
#include "../builder/builder.h"

#include "../Utils/time.h"
#include "../Utils/fps_manager.h"
#include "../Utils/Graphics/font_renderer.hpp"
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

inline static constexpr int frame_smoothing = 10;
inline const static bool full_screen = false;  // Change this value to toggle fullscreen mode
inline const static double resize_shrinkage = 0.95;

class Simulation : SimulationSettings, UI_Settings, TextSettings
{
	// Get the desktop resolution
	static sf::VideoMode getAdjustedVideoMode()
	{
		sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
		unsigned new_width = static_cast<unsigned>(desktop.width * resize_shrinkage);  // 80% of screen width
		unsigned new_height = static_cast<unsigned>(desktop.height * resize_shrinkage); // 80% of screen height

		return sf::VideoMode(new_width, new_height);
	}

	sf::VideoMode videoMode = full_screen ? sf::VideoMode::getDesktopMode() : getAdjustedVideoMode();
	sf::Uint32 windowStyle = full_screen ? sf::Style::Fullscreen : (sf::Style::Titlebar | sf::Style::Close);
	sf::RenderWindow m_window_{videoMode, "Project ARIA", windowStyle};

	FrameRateSmoothing<frame_smoothing> m_clock_{};
	Camera camera_{&m_window_, 1.f};
	Builder m_builder_{&m_window_};

	// managing time
	StopWatch m_delta_time_{};
	float m_total_time_elapsed_ = 0.f;
	unsigned m_ticks_ = 0;

	World m_world_{};

	// user controlled variables
	bool m_rendering_ = true;
	bool m_debug_ = false;

	bool m_tick_frame = false;
	bool m_tick_frame_time = false;

	LineGraph<line_maximum_data, line_x_axis_increments> protozoa_population_graph_{ &m_window_, protozoa_graph_bounds };
	LineGraph<line_maximum_data, line_x_axis_increments> food_population_graph_{ &m_window_, food_graph_bounds };

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