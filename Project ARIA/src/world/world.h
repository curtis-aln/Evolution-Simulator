#pragma once

#include <SFML/Graphics/RenderWindow.hpp>

#include "../Protozoa/Protozoa.h"

#include "../settings.h"


#include "../Utils/thread_pool.h"
#include "../Utils/o_vector.hpp"
#include "../Utils/Graphics/Circle.h"
#include "../Utils/Graphics/CircleBatchRenderer.h"
#include "../food_manager.h"

#include "../Utils/Graphics/simple_spatial_grid.h"

#include "../Utils/Graphics/SFML_Grid.h"

// The World class manages the entire simulation environment, including protozoa, food, and rendering.
// It handles updates, collisions, reproduction, and rendering of all entities within the worl.
class World : WorldSettings
{
	// render window is created in the simulation class and passed down here
	sf::RenderWindow* m_window_ = nullptr;

	Protozoa::Protozoa_Vector all_protozoa_{};

	// the world is circular to avoid protozoa getting stuck in corners. We also have a rectangular bounds for the spatial hash grid
	Circle world_circular_bounds_{ {bounds_radius, bounds_radius}, bounds_radius };
	sf::FloatRect world_rect_bounds_{ 0.f, 0.f, bounds_radius * 2.f, bounds_radius * 2.f };
	sf::VertexArray world_border_renderer_{};

	// We use these Object of Arrays approach to efficiently render large numbers of protozoa cells.
	std::vector<sf::Color> outer_color_data_{};
	std::vector<sf::Color> inner_color_data_{};
	std::vector<sf::Vector2f> position_data_{};

	// Each Cell is drawn with a simple outer and inner circle, we use batch rendering to draw all cells efficiently.
	CircleBatchRenderer outer_circle_renderer_{ m_window_ };
	CircleBatchRenderer inner_circle_renderer_{ m_window_ };

	// This handles all food related tasks like spawning, rendering, and updating food items.
	FoodManager food_manager_{ m_window_, &world_circular_bounds_ };

	// for our collision detection we use a spatial hash grid to see what cells are nearby others
	SimpleSpatialGrid<cells_x, cells_y> spatial_hash_grid_{world_rect_bounds_};
	SFML_Grid cell_grid_renderer; // renders the cell spatial hash grid

	std::vector<Cell*> temp_cells_container;

	int ticks = 0;

	tp::ThreadPool thread_pool;

	// statistics
	int average_cells_per_protozoa;
	int average_offspring_count;
	int average_mutation_rate;
	int average_mutation_range;


public:
	bool simple_mode = false;
	bool debug_mode = false;
	bool skeleton_mode = false;
	bool paused = false;
	bool draw_cell_grid = false;
	bool draw_food_grid = false;
	bool toggle_collisions = true;
	bool show_connections = true;
	bool show_bounding_boxes = true;

	Protozoa* selected_protozoa = nullptr;

	World(sf::RenderWindow* window = nullptr);

	void update_world(bool pause);

	// cell collision handling
	std::array<int, cell_capacity * 9> nearby_ids = {};

	void optimized_cell_collisions();
	void update_grid_cell(const int grid_cell_id);

	void update_protozoa_cell(int protozoa_cell_index, int neighbours_size) const;

	void update_nearby_container(int& neighbours_size, int32_t neighbour_index_x, int32_t neighbour_index_y, bool check_x, bool check_y);


	void update_protozoas();
	Protozoa* find_an_offspring();
	void reproduce_protozoa(Protozoa* protozoa);
	void update_cells_container();
	void handle_extinction_event();
	void add_cells_to_hash_grid();
	void update_debug(sf::Vector2f mouse_position) const;
	void render_world();
	void render_protozoa();
	void update_position_data();
	void check_hovering(bool debug_mode, sf::Vector2f mouse_position, bool mouse_pressed) const;
	bool check_pressed(sf::Vector2f mouse_position);
	void de_select_protozoa() const;

	float calculate_average_generation() const;

	int get_protozoa_count() const
	{
		return all_protozoa_.size();
	}

	int get_food_count() const
	{
		return food_manager_.get_size();
	}

private:
	void init_organisms();
	static void init_food();
	static void init_environment();
};