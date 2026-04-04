#pragma once
#include "Utils/utility_SFML.h"
#include "Utils/Graphics/font_renderer.hpp"
#include <cstdint>
#include "Protozoa/genetics/CellGenome.h"

struct TextSettings
{
	// locations for the fonts
	inline static const std::string bold_font_location = "media/fonts/Roboto-Bold.ttf";
	inline static const std::string regular_font_location = "media/fonts/Roboto-Regular.ttf";

	// Universal font sizes - only these many fonts should be created
	static constexpr unsigned title_font_size = 30;
	static constexpr unsigned regular_font_size = 20;
	static constexpr unsigned cell_statistic_font_size = 15;

};


struct SimulationSettings
{

public:
	inline static constexpr int frame_smoothing = 30;
	inline static constexpr bool full_screen = false;  // Change this value to toggle fullscreen mode
	inline static constexpr double resize_shrinkage = 0.95;

	inline static const std::string simulation_name = "Project A.R.I.A";
	static constexpr bool vsync = false;

	static constexpr unsigned frame_rate = 0;

	static constexpr unsigned line_maximum_data = 200;
	static constexpr unsigned line_x_axis_increments = 20;
};

struct GraphicalSettings
{
	inline static const sf::Color window_color = { 0, 0, 0 };


	static constexpr float spring_thickness = 9.f;
	static constexpr float spring_outline_thickness = 6.f;

	static constexpr float cell_outline_thickness = 30.f;

	static constexpr std::uint8_t food_transparency = 200;

	inline static const std::vector<sf::Color> food_fill_colors = {
		{200, 30, 30, food_transparency}
	};

	inline static const std::vector<sf::Color> food_outline_colors = {
		{250, 60, 60, food_transparency}
	};
};


struct WorldSettings
{
	static constexpr float bounds_radius = 25'000;

	static constexpr unsigned max_protozoa = 50'000;
	static constexpr unsigned initial_protozoa = 2'000;

	inline static constexpr size_t cells_x = 100;
	inline static constexpr size_t cells_y = cells_x;
	inline static constexpr size_t cell_capacity = 40;

	inline static constexpr float border_repulsion_magnitude = 0.001f; // how strong it is repelled from the border
	inline static constexpr float max_speed = 30;
};

struct ProtozoaSettings
{
	static constexpr float spawn_radius = 100.f;

	static constexpr float energy_decay_rate = 0.15f; // how quickly energy decays per frame

	inline static constexpr float wander_threshold = CellGenome::radius * 30.f; // if a cell wanders too far away from the protozoa it kills the whole thing
};


struct FoodSettings
{
	inline static constexpr size_t cells_x = WorldSettings::cells_x;
	inline static constexpr size_t cells_y = WorldSettings::cells_y;
	inline static constexpr size_t cell_capacity = 6;

	static constexpr unsigned max_food = 4'000;
	static constexpr unsigned initial_food = 4'000;
	inline static constexpr float food_radius = 30.f;
	inline static constexpr float friction = 0.99f;

	inline static constexpr int food_spawn_amount = 14;
	inline static constexpr int food_spawn_interval = 2; // frames between spawns
};