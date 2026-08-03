#pragma once
#include <algorithm>
#include <iostream>

#include "../../Utils/o_vec/o_vector.hpp"
#include <SFML/Graphics.hpp>

#include "cell_manager_settings.h"

#include "../../entities/cell/cell.h"
#include "../../entities/spring/spring.h"
#include "world/world_border.h"

#include "../food_manager/food_manager.h"
#include "organism_tracker.h"
#include "../../simulation/context/sim_snapshot.h"

#include <simulation/context/sim_command.h>
#include <entities/CellMatter.h>


struct CellBodyPair
{
	uint32_t cell_id{};
	uint32_t body_id{};

	bool is_valid{};
};


/* How reproduction works, in detail
when a cell has enough energy its sets reproduce = true
the protozoa manager detects this and
- cell.reproduce = false;
- makes a create cell request

The birth manager then processes this request by
- creating a new cell
- cell.offspring_index = the index of the new cell
- a temporary spring is made between the parent cell and the new cell

This keeps happening until a spring detects that both its cell's have a valid offspring index
The spring then
- sets cell_a.connection_index = cell_b.offspring_index; 
- cell_a.spring_to_copy_index = id; this is the spring data that will be used
This tells the protozoa manager what to connect (cell_a.connection_index, cell_b.offspring_index)

The connection request is detected
 */


struct BirthRequest
{
	uint32_t parent_cell_id;
};

struct ConnectionRequest
{
	int32_t offspring_id;
	int32_t connect_to_id;
};

inline static constexpr size_t max_lifetime_samples_ = 500;
inline static constexpr int survival_rate_window_size_ = 100;


// A Class which handles all protozoa related stuff in the world. updating, collisions, reproduction, etc.
class CellManager: protected CellManagerSettings
{
	sf::RenderWindow* m_window_ = nullptr;
	WorldBorder* world_bounds_ = nullptr;

	uint8_t total_max_capacity = FoodManagerSettings::cell_max_capacity * static_cast<uint8_t>(9);
	FixedSpan<obj_idx> nearby_food_ids{ total_max_capacity };
	FixedSpan<obj_idx> nearby_cell_ids{ total_max_capacity };

	// The user can click on a protozoa to select it for debugging purposes. we store a pointer to it here.
	int32_t selected_cell_id_ = -1;

	// header
	std::deque<float> recent_lifetimes_;
	float recent_lifetimes_sum_ = 0.f; // a vector storing the lifetimes of the 500 most recent protozoa deaths, used to calculate average_lifetime_
	
	std::vector<float> distribution_{}; // a vector storing the generation of all protozoa in the world, used to calculate average generation

	// used to store requests for new protozoa to be created, and for new connections to be made between cells
	std::vector<BirthRequest> birth_requests;
	std::vector<ConnectionRequest> connection_requests;
	CellManagerStatistics statistics_{};

	// main body class is kept in the world class, we keep a pointer to it here so we can access it
	o_vector<Body>* bodies_;

	// the main data structure for storing all protozoa in the world, this is a custom vector class that allows for fast iteration over active objects
	o_vector<Cell> all_cells_;
	o_vector<Spring> all_springs_;
	o_vector<CellMatter> all_cell_matter_;

	// This builds a model around a protozoa that doesnt globally exist, so it can be monitored and learned about
	OrganismTracker protozoa_tracker_{};

	std::vector<std::function<void()>> updating_bodies_;
	BarrierThreadPool thread_pool_{ (int)WorldSettings::updating_threads };
	bool update_jobs_built_ = false;
	int  current_total_cells_ = 0;

	// this spatial grid holds new born cells so that the they can form connections between other newly born cells
	SimpleSpatialGrid new_born_cell_grid_{ WorldSettings::cells_x, WorldSettings::cells_y, WorldSettings::cell_max_capacity, WorldSettings::bounds_radius * 2.0f, WorldSettings::bounds_radius * 2.0f };
	

public:
	uint16_t max_size = static_cast<uint16_t>(10000);
	FixedSpan<cell_idx, uint16_t> select_indexes{ max_size };	


public:
	// Constructor
	CellManager(sf::RenderWindow* window, WorldBorder* world_bounds, o_vector<Body>* bodies);
	void reset_cell_manager();

	// entries
	void handle_cell_manager_event(SimCommand& cmd);
	void update(int iterations);
	void update_position_container(RenderData& rend_data, const sf::FloatRect& visible_bounds);
	void update_protozoa_tracker();
	void fill_snapshot(SimSnapshot& snapshot, sf::FloatRect& visible_bounds);

	// user input & mouse handling
	void create_new_protozoa(int count, WorldBorder* spawn_area);
	void drag_selected_cell_to_point(const sf::Vector2f& target_position, const float move_fraction);
	void remove_cells_in_radius(const sf::Vector2f& position, const float radius);
	void influence_cell_velocities_in_radii(const sf::Vector2f& position, const float radius, const int intensity);

	// data fetching
	unsigned get_cell_count() const { return all_cells_.size(); }
	unsigned get_matter_count() const { return all_cell_matter_.size(); }

	bool has_cell_with_body_id(int body_id);
	
	Cell* find_cell_by_id(const int id) { return all_cells_.at(id); }
	Cell* find_cell_at_point(const sf::Vector2f mouse_position, bool make_selected_cell);
	sf::Vector2f& get_cell_pos(int cell_id);
	Body* get_cell_body(int cell_id);
	
	const sf::Vector2f* get_selected_protozoa_pos() const;

	const o_vector<Cell>& get_all_cells() const { return all_cells_; }
	const o_vector<Spring>& get_all_springs() const { return all_springs_; }
	const o_vector<CellMatter>& get_all_cell_matter() const { return all_cell_matter_; }

	o_vector<Cell>& get_all_cells() { return all_cells_; }
	o_vector<Spring>& get_all_springs() { return all_springs_; }
	

	// selected cell management
	void deselect_cell();
	const Cell* get_selected_cell() const { return all_cells_.at(selected_cell_id_); }

	// public statistics
	CellManagerStatistics& get_statistics() { return statistics_; }
	const std::vector<float>& get_generation_distribution();
	void update_100frame_stats(int iterations);
	void update_statistics();
	

private: // only functions this class can access
	// Utility
	void gather_food_in_radius(FixedSpan<cell_idx, uint16_t>& indexes, const sf::Vector2f& position, const float radius);
	void check_for_extinction_event();

	// statistics 
	void register_death_stat(const float lifetime, const bool had_offspring);
	void register_birth_stat();
	float calculate_average_generation() const;
	void fill_render_data(RenderData& render_data, sf::FloatRect& visible_bounds);

	// updating
	void update_springs();
	void update_cells();
	void update_cell_matter();
	void update_cell(Cell* cell);
	void update_new_born_cells();

	void ensure_update_jobs_built();

	// decaying objects
	void process_newly_decaying_cell(Cell* cell);

	// newly born cells
	void add_new_cells_to_grid();
	void try_connect_newborn_cell(Cell* cell);

	// birth - springs
	int32_t create_spring(const uint32_t cell_a_id, const uint32_t cell_b_id);
	void create_weak_offspring(uint32_t parent_id);
	void apply_connection_requests();

	// birth - cells
	CellBodyPair create_cell(sf::Vector2f position = { 0, 0 }, bool random_genetics = false);
	void clone_selected_protozoa();
	void create_protozoa_from_pool(const sf::Vector2f position, const unsigned max_cells, const unsigned max_springs);
	void collect_reproduction_requests();
	void apply_birth_requests();
	
	// death
	void handle_death();
	void remove_cell(Cell* cell);
	
	
};