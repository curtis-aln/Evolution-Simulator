#pragma once

#include <SFML/Graphics/RenderWindow.hpp>

#include "../Protozoa/Protozoa.h"

#include "../settings.h"

#include "../Utils/o_vector.hpp"
#include "../Utils/random.h"
#include "../Utils/Graphics/Circle.h"
#include "../Utils/Graphics/buffer_renderer.h"
#include "../food_manager.h"

#include "../Utils/Graphics/spatial_hash_grid.h"
#include "../Utils/Graphics/SFML_Grid.h"


class World : WorldSettings
{
	sf::RenderWindow* m_window_ = nullptr;

	Protozoa::Protozoa_Vector all_protozoa{};

	Circle m_bounds_{ {bounds_radius, bounds_radius}, bounds_radius };

	sf::VertexArray border_render_{};

	// rendering
	std::vector<sf::Color> outer_color_data{};
	std::vector<sf::Color> inner_color_data{};
	std::vector<sf::Vector2f> position_data{};

	CircleBuffer outer_circle_buffer{ m_window_ };
	CircleBuffer inner_circle_buffer{ m_window_ };

	FoodManager food_manager{ m_window_, &m_bounds_ };

	// to handle collisions
	const sf::FloatRect world_bounds = { 0, 0, bounds_radius * 2, bounds_radius * 2 };
	SpatialHashGrid<cells_x, cells_y, cell_capacity> spatial_hash_grid{ world_bounds };
	SFML_Grid grid_renderer;

	std::vector<Cell*> temp_cells_container;

	int ticks = 0;

public:
	bool simple_mode = false;
	bool debug_mode = false;
	bool draw_grid = false;

	Protozoa* selected_protozoa = nullptr;

	World(sf::RenderWindow* window = nullptr);

	void update_world();
	void update_hash_grid();
	void update_debug(sf::Vector2f mouse_position);
	void render_world();
	void render_protozoa();
	void update_position_data();
	void check_hovering(bool debug_mode, sf::Vector2f mouse_position, bool mouse_pressed);
	bool check_pressed(sf::Vector2f mouse_position);
	void de_select_protozoa();

private:
	void init_organisms();
	void init_food();
	void init_environment();
};