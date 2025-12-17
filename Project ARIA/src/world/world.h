#pragma once

#include <SFML/Graphics/RenderWindow.hpp>

#include "../Protozoa/Protozoa.h"
#include "../food_manager.h"
#include "../settings.h"

#include "ProtozoaManager.h"

#include "../Utils/thread_pool.h"
#include "../Utils/o_vector.hpp"
#include "../Utils/Graphics/Circle.h"
#include "../Utils/Graphics/CircleBatchRenderer.h"
#include "../Utils/Graphics/simple_spatial_grid.h"
#include "../Utils/Graphics/SFML_Grid.h"

// The World class manages the entire simulation environment, including protozoa, food, and rendering.
// It handles updates, collisions, reproduction, and rendering of all entities within the worl.
class World : public ProtozoaManager
{
	// render window is created in the simulation class and passed down here
	sf::RenderWindow* m_window_ = nullptr;

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



	// Tracking the number of iterations have passed in this world
	int iterations_ = 0;

	// thread pool for parallel processing of the update loop
	tp::ThreadPool thread_pool_;

	

public:
	// Keyboard toggles for various modes
	bool simple_mode = false;        // only render outer circles of cells
	bool debug_mode = false;         // lets the user see information on any selected cell
	bool skeleton_mode = false;      // only render springs between cells
	bool paused = false;             // pauses the simulation update loop
	bool draw_cell_grid = false;     // renders the spatial hash grid for cell collision detection
	bool draw_food_grid = false;     // renders the spatial hash grid for food items
	bool toggle_collisions = true;   // enables or disables cell collision handling
	bool show_connections = true;    // shows spring connections between cells
	bool show_bounding_boxes = true; // shows bounding boxes around protozoa

	// for the simple spatial grid, we need a temporary array to store nearby cell ids
	std::array<int, cell_capacity * 9> nearby_ids = {};

public:
	World(sf::RenderWindow* window = nullptr);

	// updating functions
	void update(bool pause);
	void render();

	// fetch functions
	int get_food_count() const { return food_manager_.get_size(); }

	void check_if_mouse_is_hovering(bool debug_mode, sf::Vector2f mouse_position, bool mouse_pressed) const;
	bool handle_mouse_click(sf::Vector2f mouse_position);
	void move_cell_in_selected_protozoa(sf::Vector2f mouse_position) const;

private:
	// update functions
	void update_grid_cell(const int grid_cell_id);
	void update_protozoa_cell(int protozoa_cell_index, int neighbours_size) const;
	void update_nearby_container(int& neighbours_size, int32_t neighbour_index_x, int32_t neighbour_index_y, bool check_x, bool check_y);
	
	void update_position_container();
	void update_spatial_grid();

	// rendering functions
	void render_protozoa();
	
	// initialization functions
	void init_organisms();
	static void init_food();
	static void init_environment();

};