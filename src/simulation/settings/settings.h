#pragma once
#include "Utils/Graphics/font_renderer.hpp"
#include <cstdint>

void load_settings(const std::string& path);


struct SimulationSettings
{
	inline static int max_iterations;
	inline static float max_simulation_time;

	inline static double initial_frame_rate_updating; // set to zero to run the simulation as fast as possible
	inline static double initial_frame_rate_rendering;

	inline static constexpr int frame_smoothing = 60;
	inline static constexpr double window_resize_shrinkage = 0.95;

	inline static const std::string simulation_name = "Project A.R.I.A";
	inline static const std::string settings_file_location = "media/aria_settings.toml";

	inline static constexpr float camera_lerp_factor = 0.04f; // how quickly the camera follows the selected protozoa

	inline static const std::vector<sf::Color> bg_colors = {{0, 5, 40}};
	inline static const sf::Color window_color = { 0, 0, 0 };

	inline static bool full_screen;
	inline static bool vsync;

	inline static float ui_scale_percent;

	inline static constexpr float min_zoom_to_select_protozoa = 0.062f; // if zoomed out more than this, clicking on protozoa is disabled

};
