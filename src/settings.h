#pragma once
#include "Utils/utility_SFML.h"
#include "Utils/Graphics/font_renderer.hpp"
#include <cstdint>
#include "Protozoa/genetics/CellGenome.h"

#include <toml++/toml.hpp>




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

	inline static constexpr float ui_scale_percent = 100.f;
};

struct GraphicalSettings
{
	inline static const sf::Color window_color = { 0, 0, 0 };


	static constexpr float spring_thickness = 9.f;
	static constexpr float spring_outline_thickness = 6.f;

	static constexpr float cell_outline_thickness = 1.1f; // relative to the size of the cell

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
	inline static float bounds_radius;

	inline static unsigned max_protozoa;
	inline static unsigned initial_protozoa;

	inline static constexpr size_t cells_x = 100;
	inline static constexpr size_t cells_y = cells_x;
	inline static constexpr std::uint8_t cell_max_capacity = 25;

	inline static float border_repulsion_magnitude; // how strong it is repelled from the border
	inline static float max_speed;

	inline static bool auto_reset_on_extinction = false;
};

struct ProtozoaSettings
{
	inline static int max_cells;

	inline static float spawn_radius;

	inline static float energy_decay_rate; // how quickly energy decays per frame

	inline static float wander_threshold = 90 * 13.f; // if a cell wanders too far away from the protozoa it kills the whole thing

	inline static float breaking_length = 90 * 6.f;
	inline static float maximum_extension = 90 * 4.f;

	inline static float spring_work_const = 1.f / 100'000.f; // how we scale the energy cost of springs

	inline static size_t reproductive_cooldown;
	inline static float digestive_time = 100.f; // per cell

	inline static float initial_energy; // energy the protozoa spawn with

};


struct FoodSettings
{
	inline static constexpr size_t cells_x = WorldSettings::cells_x;
	inline static constexpr size_t cells_y = WorldSettings::cells_y;
	inline static int cell_max_capacity = 20;
	inline static size_t update_freq = 4; // food do not move that often so they dont have to be updated in the grid every frame

	static constexpr unsigned max_food = 20'000;
	static constexpr unsigned initial_food = 500;
	inline static float food_radius;
	inline static float friction;

	inline static sf::Vector3i food_darkest_color = { 0, 160, 0 };
	inline static sf::Vector3i food_lightest_color = { 80, 255, 100 };
	inline static float vibration_strength = 3.0f;

	inline static float kFoodVisibilityRampFrames = 90;
	inline static float kFoodMaxAlpha = 200;

	inline static float spawn_proportionality_constant = 0.085f; // range between [0.001, 0.01]
	inline static float food_spawn_distance = 200.0f;
	inline static size_t reproductive_cooldown;
	inline static float reproductive_threshold = 300.f; // how old a food has to be before it can reproduce

	inline static float initial_nutrients = 5;
	inline static float final_nuterients = 60;
	inline static size_t nutrient_development_time = 400;

	inline static float death_age;
	inline static float death_age_chance = 0.01f; // every frame past its death age gives it this chance of dying
};

void load_settings(const std::string& path);

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