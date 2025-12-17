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
inline Font TextSettings::title_font{ nullptr, title_font_size, bold_font_location };
inline Font TextSettings::regular_font{ nullptr, regular_font_size, regular_font_location };
inline Font TextSettings::cell_statistic_font{ nullptr, cell_statistic_font_size, regular_font_location };



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
public:
	inline static sf::Vector2f get_window_dimensions()
	{
		auto vid = sf::VideoMode::getDesktopMode();
		sf::Vector2u size_int = { vid.width, vid.height };
		auto size = static_cast<sf::Vector2f>(size_int);
		if (full_screen)
			return size;
		return size * static_cast<float>(resize_shrinkage);
	}


private:
	inline static const sf::Vector2f winDims = get_window_dimensions();
	inline static const float middle_x = winDims.x / 2;
	inline static const float line_graph_buffer = winDims.x / 10;
	inline static const float line_graph_width = middle_x - line_graph_buffer;
	inline static const float line_graph_height = winDims.y / 8;
	inline static const float line_graph_y = winDims.y - line_graph_height - line_graph_buffer/2;

public:
	inline static constexpr int frame_smoothing = 10;
	inline static constexpr bool full_screen = false;  // Change this value to toggle fullscreen mode
	inline static constexpr double resize_shrinkage = 0.95;

	inline static const std::string simulation_name = "Project A.R.I.A";
	static constexpr bool vsync = false;

	static constexpr unsigned frame_rate = 60;

	inline static const sf::Color window_color = {0, 0, 0};


	// statistics settings
	// The two line graphs should exist at the bottom of the screen starting from the middle going out 5/6ths
	// of the way to the border.
	
	static constexpr unsigned line_maximum_data = 150;
	static constexpr unsigned line_x_axis_increments = 100;

	inline static const sf::FloatRect protozoa_graph_bounds = 
	{ line_graph_buffer/2, line_graph_y, line_graph_width, line_graph_height };

	inline static const sf::FloatRect food_graph_bounds = 
	{ line_graph_buffer * 1.5f + line_graph_width, line_graph_y, line_graph_width , line_graph_height };

	inline static const sf::FloatRect text_renderer_bounds  = { 100, 100, 500, 500 };

	inline static const sf::Color protozoa_graph_line_color = { 20, 200, 20 };
	inline static const sf::Color protozoa_under_graph_color = { 20, 100, 20 };
	inline static const sf::Color food_graph_line_color = { 200, 20, 20 };
	inline static const sf::Color food_under_graph_color = { 100, 20, 20 };
};


struct WorldSettings
{
	static constexpr float bounds_radius = 30'000;

	static constexpr unsigned max_protozoa = 30'000;
	static constexpr unsigned initial_protozoa = 300;

	inline static constexpr size_t cells_x = 50;
	inline static constexpr size_t cells_y = 50;
	inline static constexpr size_t cell_capacity = 18;
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
	inline static constexpr size_t cells_x = 80;
	inline static constexpr size_t cells_y = 80;
	inline static constexpr size_t cell_capacity = 20;

	static constexpr unsigned max_food = 80'000;
	static constexpr unsigned initial_food = 10'000;
	inline static constexpr float food_radius = 30.f;
	inline static constexpr float friction = 0.99f;

	static constexpr sf::Uint8 transparency = 200;
	inline static const std::vector<sf::Color> fill_colors = {
		{200, 30, 30, transparency}
	};

	inline static const std::vector<sf::Color> outline_colors = {
		{250, 60, 60, transparency}
	};

};


#include <unordered_map>
#include <vector>