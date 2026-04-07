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
#include "../Utils/Graphics/spatial_grid/simple_spatial_grid.h"
#include "../Utils/Graphics/SFML_Grid.h"

// The World class manages the entire simulation environment, including protozoa, food, and rendering.
// It handles updates, collisions, reproduction, and rendering of all entities within the worl.
class World : public ProtozoaManager
{
	// render window is created in the simulation class and passed down here
	sf::RenderWindow* m_window_ = nullptr;

	// the world is circular to avoid protozoa getting stuck in corners. We also have a rectangular bounds for the spatial hash grid
	Circle world_circular_bounds_{ {bounds_radius, bounds_radius}, bounds_radius };
	sf::FloatRect world_rect_bounds_{ {0.f, 0.f}, {bounds_radius * 2.f, bounds_radius * 2.f } };
	sf::VertexArray world_border_renderer_{};

	// We use these Object of Arrays approach to efficiently render large numbers of protozoa cells.
	std::vector<sf::Color> outer_color_data_{};
	std::vector<sf::Color> inner_color_data_{};
	std::vector<sf::Vector2f> position_data_{};
	int entity_count = 0; // how many cells are currently in the world
	

	// Each Cell is drawn with a simple outer and inner circle, we use batch rendering to draw all cells efficiently.
	CircleBatchRenderer outer_circle_renderer_{ m_window_ };
	CircleBatchRenderer inner_circle_renderer_{ m_window_ };

	// This handles all food related tasks like spawning, rendering, and updating food items.
	FoodManager food_manager_{ m_window_, &world_circular_bounds_ };

	// for our collision detection we use a spatial hash grid to see what cells are nearby others
	SimpleSpatialGrid spatial_hash_grid_{ cells_x, cells_y, cell_max_capacity, bounds_radius * 2.f, bounds_radius * 2.f };
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
	bool track_statistics = true;

	// for the simple spatial grid, we need a temporary array to store nearby cell ids
	std::array<int, cell_max_capacity * 9> nearby_ids = {};

	float min_speed = 0;
	float delta_min_speed = 0.0;

	// statistics
	float average_generation_ = 0.f; // The average generation of all protozoa

	float frames_per_generation_ = -1.f; // The amount of frames it takes for a new generation to be born, negative values represent undefined
	float tracked_generation_ = 0.f; // The generation we are currently tracking, used to calculate frames_per_generation_
	float frames_since_last_gen_change = 0.f; // The time when the tracked generation was born, used to calculate frames_per_generation_

	float average_cells_per_protozoa_ = 0.f;
	float average_offspring_count_ = 0.f;
	float average_mutation_rate_ = 0.f;
	float average_mutation_range_ = 0.f;

	
	
	
	World(sf::RenderWindow* window = nullptr);

	// updating functions
	void update();
	SimpleSpatialGrid* get_spatial_grid();
	SimpleSpatialGrid* get_food_spatial_grid();
	FoodManager* get_food_manager();
	void render(Font* font);

	// fetch functions
	int get_food_count() const { return food_manager_.get_size(); }

	bool handle_mouse_click(sf::Vector2f mouse_position);
	void keyboardEvents(const sf::Keyboard::Key& event_key_code);

private:
	// update functions
	void update_grid_cell(const int grid_cell_id);
	void update_protozoa_cell(int protozoa_cell_index, int neighbours_size);
	void update_nearby_container(int& neighbours_size, int32_t neighbour_index_x, int32_t neighbour_index_y);
	
	void update_position_container();
	void update_statistics();

	// rendering functions
	void render_protozoa(Font* font);
	
	// initialization functions
	void init_organisms();

};