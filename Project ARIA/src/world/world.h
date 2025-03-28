#pragma once

#include <SFML/Graphics/RenderWindow.hpp>

#include "../Protozoa/Protozoa.h"

#include "../settings.h"


#include "../Utils/thread_pool.h"
#include "../Utils/o_vector.hpp"
#include "../Utils/random.h"
#include "../Utils/Graphics/Circle.h"
#include "../Utils/Graphics/buffer_renderer.h"
#include "../food_manager.h"

#include "../Utils/Graphics/spatial_hash_grid.h"
#include "../Utils/Graphics/simple_spatial_grid.h"

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
	SimpleSpatialGrid<cells_x, cells_y> spatial_hash_grid{ world_bounds };
	SFML_Grid cell_grid_renderer;
	SFML_Grid food_grid_renderer;

	std::vector<Cell*> temp_cells_container;

	int ticks = 0;

	tp::ThreadPool thread_pool;

public:
	bool simple_mode = false;
	bool debug_mode = false;
	bool skeleton_mode = false;
	bool paused = false;
	bool draw_cell_grid = false;
	bool draw_food_grid = false;
	bool toggle_collisions = true;

	Protozoa* selected_protozoa = nullptr;

	World(sf::RenderWindow* window = nullptr);

	void update_world(bool pause);

	// cell collision handling
	std::array<int, cell_capacity * 9> nearby_ids = {};

	void optimized_cell_collisions();
	void update_grid_cell(const int grid_cell_id);

	void update_protozoa_cell(int protozoa_cell_index, int neighbours_size);

	void update_nearby_container(int& neighbours_size, int32_t neighbour_index_x, int32_t neighbour_index_y, bool check_x, bool check_y);


	void update_protozoas();
	Protozoa* find_an_offspring();
	void reproduce_protozoa(Protozoa* protozoa);
	void update_cells_container();
	void handle_extinction_event();
	void add_cells_to_hash_grid();
	void update_debug(sf::Vector2f mouse_position);
	void render_world();
	void render_protozoa();
	void update_position_data();
	void check_hovering(bool debug_mode, sf::Vector2f mouse_position, bool mouse_pressed);
	bool check_pressed(sf::Vector2f mouse_position);
	void de_select_protozoa();

	float calculate_average_generation();

	int get_protozoa_count()
	{
		return all_protozoa.size();
	}

	int get_food_count()
	{
		return food_manager.get_size();
	}

private:
	void init_organisms();
	void init_food();
	void init_environment();
};