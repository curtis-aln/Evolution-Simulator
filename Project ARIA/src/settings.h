#pragma once


struct SimulationSettings
{
	inline static const std::string simulation_name = "Project A.R.I.A";

	static constexpr unsigned frame_rate = 3000;

	static constexpr unsigned window_width = 1500;
	static constexpr unsigned window_height = 800;

	inline static const sf::Color window_color = sf::Color::Black;
};


struct WorldSettings
{
	inline static constexpr float bounds_radius = 4500.f;

	static constexpr unsigned max_protozoa = 250;
	static constexpr unsigned initial_protozoa = max_protozoa / 2;

	static constexpr unsigned max_food = 200;
	static constexpr unsigned initial_food = max_food / 2;

};


struct ProtozoaSettings
{
	static constexpr unsigned max_cells = 30;

	inline static constexpr float spring_thickness = 3.f;
	inline static constexpr float spring_outline_thickness = 2.f;
};


struct CellSettings
{
	inline static constexpr sf::Uint8 transparancy = 230;

	inline static const std::vector<sf::Color> cell_fill_colors = {
		{255, 204, 204, transparancy},
		{208, 255, 194, transparancy},
		{218, 194, 255, transparancy}
	};

	inline static const std::vector<sf::Color> cell_outline_colors = {
		{255, 235, 235, transparancy},
		{234, 247, 255, transparancy},
		{226, 255, 216, transparancy},
		{229, 219, 255, transparancy}
	};
	inline static const std::vector<sf::Color> spring_fill_colors = cell_fill_colors;
	inline static const std::vector<sf::Color> spring_outline_colors = cell_outline_colors;

	inline static constexpr float cell_radius = 10.f;
	inline static constexpr float cell_outline_thickness = 2.f;
};


struct BuilderSettings
{
	inline static const sf::Color outline_color = { 150, 150, CellSettings::transparancy };
	inline static const sf::Color fill_color = { 30, 30, 30, CellSettings::transparancy };

	//inline static const sf::FloatRect bounds = { WorldSettings::bounds_radius/1.5, WorldSettings::bounds_radius/1.5, 400, 300 };
	inline static const sf::FloatRect bounds = {200, 200, 700, 450 };
};


struct FoodManagerSettings
{
	inline static constexpr unsigned max_food_clusters = 60;
};


struct FoodSettings
{
	inline static constexpr sf::Uint8 transparency = 200;
	inline static const std::vector<sf::Color> fill_colors = {
		{200, 30, 30, transparency}
	};

	inline static const std::vector<sf::Color> outline_colors = {
		{250, 60, 60, transparency}
	};

	inline static constexpr unsigned max_cluster_size = 8;
};