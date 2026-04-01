#pragma once
#include <SFML/Graphics.hpp>
#include "../settings.h"
#include "../world/world.h"

#include "../Utils/time.h"
#include "../Utils/fps_manager.h"
#include "../Utils/UI/Camera.hpp"

// multithreading imports, rendering and updating happens on two separate threads, with rendering on the main thread
#include <thread>
#include <mutex>
#include <atomic>
#include <cstdint>

#include <imgui.h>
#include <imgui-SFML.h>
#include <implot.h>


class Simulation : SimulationSettings, UI_Settings, TextSettings
{
	// Get the desktop resolution but shrinks it by 80%
	static sf::VideoMode getAdjustedVideoMode()
	{
		sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
		unsigned new_width = static_cast<unsigned>(desktop.size.x * resize_shrinkage);  // 80% of screen width
		unsigned new_height = static_cast<unsigned>(desktop.size.y * resize_shrinkage); // 80% of screen height
		std::cout << "new width height: " << new_width << ", " << new_height << "\n";
		return sf::VideoMode({new_width, new_height});
	}

	Font title_font{ nullptr, title_font_size, bold_font_location };
	Font regular_font{ nullptr, regular_font_size, regular_font_location };
	Font cell_statistic_font{ nullptr, cell_statistic_font_size, regular_font_location };

	sf::VideoMode videoMode = full_screen ? sf::VideoMode::getDesktopMode() : getAdjustedVideoMode();
	std::uint32_t windowStyle = sf::Style::None;
	sf::RenderWindow m_window_{videoMode, "Project ARIA", windowStyle};

	FrameRateSmoothing<frame_smoothing> m_clock_{};
	Camera camera_{&m_window_, 1.f};

	// managing time
	StopWatch m_delta_time_{};
	float m_total_time_elapsed_ = 0.f;
	unsigned m_ticks_ = 0;

	World m_world_{};

	// user controlled variables
	bool m_rendering_ = true;
	bool hide_panels = false;

	bool m_tick_frame = false;
	bool m_tick_frame_time = false;

	bool mouse_pressed_event = false;
	bool running = true;
	float fps_ = 0;

	// ImPlot
	static constexpr int graph_history = 40'000;
	std::vector<float> time_history_;
	std::vector<float> protozoa_history_;
	std::vector<float> food_history_;
	int   graph_offset_ = 0;

	ImPlotColormap m_plot_colormap_{};

	int graph_count_ = 0; // how many samples have been recorded


public:
	Simulation();

	void run_simulation();

private:
	void init_imGUI();

	// updating

	void update_one_frame();
	void camera_follow_selected_protozoa();
	void update_line_graphs();

	void imgui_stats_panel();
	void imgui_controls_panel();
	void imgui_tuning_panel();
	void imgui_population_graph();

	// rendering
	void draw_everything();
	void handle_imGUI();
	void render();
	void manage_frame_rate();

	void imgui_debug_panel(Cell* selected_cell, Protozoa* selected_protozoa);

	// events 
	void handle_events();
	bool try_select_protozoa(const sf::Vector2f& cam_pos);
	void handle_mouse_press(const sf::Vector2f& cam_pos);
	void handle_mouse_release();
	void handle_hover(const sf::Vector2f& cam_pos);
	void handle_pause_toggle();
	void handle_keyboard_events(const sf::Keyboard::Key& event_key_code);
	void dispatch_event(const sf::Event& event, const sf::Vector2f& cam_pos);
};