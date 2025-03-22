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
	static constexpr unsigned line_x_axis_increments = 3;
	inline static const sf::FloatRect protozoa_graph_bounds = { 2600, 200, 2100, 600 };
	inline static const sf::FloatRect food_graph_bounds = { 2600, 1000, 2100, 600 };
	inline static const sf::FloatRect text_renderer_bounds = { 2600, 1700, 900, 1200 };

	inline static const sf::Color protozoa_graph_line_color = { 20, 200, 20 };
	inline static const sf::Color protozoa_under_graph_color = { 20, 100, 20 };
	inline static const sf::Color food_graph_line_color = { 200, 20, 20 };
	inline static const sf::Color food_under_graph_color = { 100, 20, 20 };
};


struct WorldSettings
{
	static constexpr float bounds_radius = 90'000;

	static constexpr unsigned max_protozoa = 15'000;
	static constexpr unsigned initial_protozoa = 7'500;

	inline static constexpr size_t cells_x = 200;
	inline static constexpr size_t cells_y = 200;
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
	inline static const sf::Vector2i cell_amount_range = { 2, 4 };

	inline static const sf::Vector2f rest_length_ranges = { 0., 1.f }; // 0 is a dist of cell radius and 1 is a dist of cell radius * 4
	inline static const float rest_length_val = CellSettings::cell_radius;
	inline static const sf::Vector2f friction_range = { 0.90f, 0.998f };

	inline static const sf::Vector2f damping_const_range = { 0.975f, 0.995f };
	inline static const sf::Vector2f spring_const_range = { 0.01f, 0.06f };
	inline static const sf::Vector2f spring_vibration_range = { 30, 200 }; // how often the spring length changes from minimum to maximum
	inline static const sf::Vector2f cell_vibration_range   = { 30, 200 };
	
	static constexpr sf::Uint8 transparancy = 200;

	inline static const std::vector<sf::Color> inner_colors = {
		{255, 204, 204, transparancy},
		{208, 255, 194, transparancy},
		{218, 194, 255, transparancy}
	};

	inline static const std::vector<sf::Color> outer_colors = {
		{255, 235, 235, transparancy},
		{234, 247, 255, transparancy},
		{226, 255, 216, transparancy},
		{229, 219, 255, transparancy}
	};
};


struct ProtozoaSettings
{
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
	inline static constexpr size_t cells_x = 200;
	inline static constexpr size_t cells_y = 200;
	inline static constexpr size_t cell_capacity = 20;

	static constexpr unsigned max_food = 45'000;
	static constexpr unsigned initial_food = 45'000;
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

