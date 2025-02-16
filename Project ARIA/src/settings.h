#pragma once
#include "Utils/utility_SFML.h"

struct TextSettings
{
	inline static const std::string bold_font_loc = "src/Utils/fonts/Roboto-Bold.ttf";
	inline static const std::string regular_font_loc = "src/Utils/fonts/Roboto-Regular.ttf";
	static constexpr unsigned t_title_size = 70;
	static constexpr unsigned t_regular_size = 40;
	static constexpr unsigned t_small_size = 20;
};


struct ButtonSettings
{
	static constexpr sf::Uint8 transparency = 220;
	inline static const sf::Color b_default_color = { 30, 30, 30, transparency };
	inline static const sf::Color b_active_color = { 50, 50, 50, transparency };
	inline static const sf::Color b_outline_color = { 200, 200, 200, transparency };

	static constexpr unsigned b_font_size = TextSettings::t_regular_size;
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

	static constexpr unsigned frame_rate = 144;

	static constexpr unsigned window_width = 4500;
	static constexpr unsigned window_height = 2800;

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
	static constexpr float bounds_radius = 13000.f;

	static constexpr unsigned max_protozoa = 40;
	static constexpr unsigned initial_protozoa = max_protozoa / 2;
};


                                                                                                                  
struct SpringSettings
{
	static constexpr float rest_length = 120.f;
	static constexpr float spring_constant = 0.75f;//0.08f;
	static constexpr float damping_factor = 0.50f;
};


struct GeneticInformationSettings
{
	inline static const sf::Vector2i init_cell_count_range = { 2, 6 };
	inline static const sf::Vector2f init_cell_radius_range = { 30.f, 50.f };

	inline static const sf::Vector2f init_rest_length_range = { init_cell_radius_range.y, init_cell_radius_range.y * 4.f };
	inline static const sf::Vector2f damping_const_range = { 0.95f, 0.99f };
	inline static const sf::Vector2f spring_const_range = { 0.01f, 0.05f };

	static constexpr sf::Uint8 transparancy = 230;

	inline static const std::vector<sf::Color> fill_colors = {
		{255, 204, 204, transparancy},
		{208, 255, 194, transparancy},
		{218, 194, 255, transparancy}
	};

	inline static const std::vector<sf::Color> outline_colors = {
		{255, 235, 235, transparancy},
		{234, 247, 255, transparancy},
		{226, 255, 216, transparancy},
		{229, 219, 255, transparancy}
	};
};


struct ProtozoaSettings
{
	static constexpr unsigned max_cells = 30;

	static constexpr float spring_thickness = 9.f;
	static constexpr float spring_outline_thickness = 6.f;
};


struct CellSettings
{
	static constexpr float cell_outline_thickness = 12.f;
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


struct FoodManagerSettings
{
	static constexpr unsigned max_food_clusters = 60;
};


struct FoodSettings
{
	static constexpr sf::Uint8 transparency = 200;
	inline static const std::vector<sf::Color> fill_colors = {
		{200, 30, 30, transparency}
	};

	inline static const std::vector<sf::Color> outline_colors = {
		{250, 60, 60, transparency}
	};

	static constexpr unsigned max_cluster_size = 8;
};