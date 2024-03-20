#pragma once


struct Colors
{
	
};


struct SimulationSettings
{
	inline static const std::string simulation_name = "Project A.R.I.A";

	static constexpr unsigned frame_rate = 30;

	static constexpr unsigned window_width = 1500;
	static constexpr unsigned window_height = 800;

	inline static const sf::Color window_color = sf::Color::Black;
};


struct WorldSettings
{
	static constexpr unsigned max_protozoa = 200;
	static constexpr unsigned initial_protozoa = max_protozoa / 2;

	static constexpr unsigned max_food = 200;
	static constexpr unsigned initial_food = max_food / 2;

	sf::Rect<float> world_boundaries = { 0, 0, SimulationSettings::window_width, SimulationSettings::window_height };
};


struct ProtozoaSettings
{
	static constexpr unsigned max_cells = 16;
};


struct CellSettings
{
	
};