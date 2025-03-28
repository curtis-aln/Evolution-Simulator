#pragma once
#include "Utils/utility_SFML.h"
#include "Utils/Graphics/font_renderer.hpp"

struct TextSettings
{
	// locations for the fonts
	inline static const std::string bold_font_location = "media/fonts/Roboto-Bold.ttf";
	inline static const std::string regular_font_location = "media/fonts/Roboto-Regular.ttf";

	// Universal font sizes - only these many fonts should be created
	static constexpr unsigned title_font_size = 70;
	static constexpr unsigned regular_font_size = 40;
	static constexpr unsigned cell_statistic_font_size = 20;

	static Font title_font;
	static Font regular_font;
	static Font cell_statistic_font;
};

// Initialize static Font members with appropriate parameters - Which needs to happen outside the class
inline Font TextSettings::title_font{ nullptr, TextSettings::title_font_size, TextSettings::bold_font_location };
inline Font TextSettings::regular_font{ nullptr, TextSettings::regular_font_size, TextSettings::regular_font_location };
inline Font TextSettings::cell_statistic_font{ nullptr, TextSettings::cell_statistic_font_size, TextSettings::regular_font_location };



struct ButtonSettings
{
	static constexpr sf::Uint8 transparency = 220;
	inline static const sf::Color b_default_color = { 30, 30, 30, transparency };
	inline static const sf::Color b_active_color = { 50, 50, 50, transparency };
	inline static const sf::Color b_outline_color = { 200, 200, 200, transparency };

	static constexpr unsigned b_font_size = TextSettings::regular_font_size;
	static constexpr float b_thickness = 4;
};


struct UI_Settings
{
	inline static const sf::Color border_fill_color = { 30, 30, 30 };
	inline static const sf::Color border_outline_color = { 50, 50, 50 };
	static constexpr float border_outline_thickness = 12;
	static constexpr sf::Uint8 transparency = 220;
};

struct SimulationSettings
{
	inline static const std::string simulation_name = "Project A.R.I.A";
	static constexpr bool vsync = false;

	static constexpr unsigned frame_rate = 60;

	inline static const sf::Color window_color = {0, 0, 0};


	// statistics settings
	static constexpr unsigned line_maximum_data = 70;
	static constexpr unsigned line_x_axis_increments = 20;
	inline static const sf::FloatRect protozoa_graph_bounds = { 700, 1500, 1000, 300 };
	inline static const sf::FloatRect food_graph_bounds     = { 2000, 1500, 1000, 300 };
	inline static const sf::FloatRect text_renderer_bounds  = { 2600, 1700, 900, 1200 };

	inline static const sf::Color protozoa_graph_line_color = { 20, 200, 20 };
	inline static const sf::Color protozoa_under_graph_color = { 20, 100, 20 };
	inline static const sf::Color food_graph_line_color = { 200, 20, 20 };
	inline static const sf::Color food_under_graph_color = { 100, 20, 20 };
};


struct WorldSettings
{
	static constexpr float bounds_radius = 140'000;

	static constexpr unsigned max_protozoa = 30'000;
	static constexpr unsigned initial_protozoa = 15'000;

	inline static constexpr size_t cells_x = 240;
	inline static constexpr size_t cells_y = 240;
	inline static constexpr size_t cell_capacity = 16;
};


                                                                                                                  
struct SpringSettings
{
	static constexpr float rest_length = 120.f;
	static constexpr float spring_constant = 0.75f;//0.08f;
	static constexpr float damping_factor = 0.50f;
};

struct CellSettings
{
	static constexpr float cell_radius = 90.f;
	static constexpr float cell_outline_thickness = 30.f;
};


struct GeneSettings
{
	// the amount of cells each protzoa starts off with
	inline static const sf::Vector2i cell_amount_range = { 4, 6 };

	// chances of adding or removing a cell per mutation event
	inline static constexpr float add_cell_chance = 0.1f;
	inline static constexpr float remove_cell_chance = 0.03f;

	inline static constexpr float delta_mutation_rate  = 0.0075f;
	inline static constexpr float delta_mutation_range = 0.0075f;
	
};

struct SpringGeneSettings
{
	inline static constexpr float init_damping = 0.975f;
	inline static constexpr float init_spring_const = 0.26f;

	inline static const sf::Vector2f offset_range = { 0.f, pi };
	inline static const sf::Vector2f frequency_range = { 0.f, pi };

	inline static constexpr float minLength = CellSettings::cell_radius * 2.f;
	inline static constexpr float maxLength = minLength * 2.5f;
};


struct CellGeneSettings
{
	// cell friction settings
	inline static const sf::Vector2f offset_range = { 0.f, pi };
	inline static const sf::Vector2f frequency_range = { 0.f, pi };

	inline static constexpr float minFriction = 0.992f;
	inline static constexpr float maxFriction = 1.00f;

	static constexpr sf::Uint8 transparancy = 160;
};


struct ProtozoaSettings
{
	static constexpr float spawn_radius = 100.f;

	static constexpr float spring_thickness = 9.f;
	static constexpr float spring_outline_thickness = 6.f;
};




struct BuilderSettings
{
	inline static const sf::FloatRect bounds = {200, 200, 2100, 1350 };
	inline static const sf::FloatRect protozoa_space = resize_rect(bounds, {75, 180});
	static constexpr float button_spacing = 60;

	inline static const sf::Vector2f button_size = { 240, 90 };
	inline static const sf::Vector2f button_buffer = { 150, 60 };
	inline static const sf::Vector2f title_buffer = { 0, 60 };
	inline static const sf::Vector2f instructions_buffer = { 100, 30 };

	inline static const std::string box_title = "Protozoa Factory";
	inline static const std::string box_instructions = "Left Click - Select \nLeft hold - Drag \nRight Hold - Add link";

	
};


struct FoodSettings
{
	inline static constexpr size_t cells_x = 240;
	inline static constexpr size_t cells_y = 240;
	inline static constexpr size_t cell_capacity = 25;

	static constexpr unsigned max_food = 80'000;
	static constexpr unsigned initial_food = 80'000;
	inline static const float food_radius = 30.f;
	inline static const float friction = 0.99f;

	static constexpr sf::Uint8 transparency = 200;
	inline static const std::vector<sf::Color> fill_colors = {
		{200, 30, 30, transparency}
	};

	inline static const std::vector<sf::Color> outline_colors = {
		{250, 60, 60, transparency}
	};

};


#include <iostream>
#include <unordered_map>
#include <vector>

#include <unordered_map>

#include <vector>
#include <utility>

struct GeneticPresets
{
	using Preset = std::vector<std::pair<int, int>>;

	Preset two_celled_protozoa = {
		{0, 1}
	};

	Preset three_celled_protozoa = {
		{0, 1},
		{0, 2},
		{1, 2}
	};

	Preset five_celled_protozoa = {
		{0, 1},
		{0, 2},
		{0, 3},
		{0, 4}
	};

	std::vector<Preset> presets = {
		two_celled_protozoa,
		three_celled_protozoa,
		five_celled_protozoa
	};
};
