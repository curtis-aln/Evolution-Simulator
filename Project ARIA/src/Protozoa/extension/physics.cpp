#include "../Protozoa.h"
#include "../../Utils/random.h"
#include <stdexcept> // Required for std::runtime_error

#include "../../Utils/Graphics/spatial_hash_grid.h"
#include "../../food_manager.h"


Protozoa::Protozoa(int id_, Circle* world_bounds, sf::RenderWindow* window, const bool init_cells)
	: id(id_), m_window_(window), m_world_bounds_(world_bounds), GeneticPresets()
{

	food_positions_nearby.reserve(30);
	cell_positions_nearby.reserve(30);

	if (world_bounds == nullptr)
	{
		return;
	}

	if (init_cells)
	{
		const auto idx = Random::rand_range(size_t(0), presets.size() - 1);
		load_preset(presets.at(idx));
		//initialise_cells();
		//initialise_springs();
	}
}


void Protozoa::generate_random()
{
	m_cells_.clear();
	m_springs_.clear();
	initialise_cells();
	initialise_springs();
}

void Protozoa::initialise_cells()
{
	// cells are spawned in clusters around an origin
	const float world_rad = m_world_bounds_->radius;
	sf::Vector2f center = Random::rand_pos_in_circle(m_world_bounds_->center, world_rad);
	const Circle protozoa_area = { center, spawn_radius };

	const int cell_count = Random::rand_range(cell_amount_range);
	m_cells_.reserve(cell_count);
	for (int i = 0; i < cell_count; ++i)
	{
		m_cells_.emplace_back(i, id, protozoa_area.rand_pos());
	}
}


void Protozoa::initialise_springs()
{
	const int cell_count = m_cells_.size();

	m_springs_.reserve(cell_count);

	for (int i = 1; i < cell_count; ++i)
	{
		m_springs_.emplace_back(i, Random::rand_range(0, i - 1));
	}
}


void Protozoa::update(FoodManager& food_manager, bool debug)
{
	if (m_cells_.empty()) // No computation is needed if there are no cells
		return;

	update_springs();
	update_cells();

	update_bounding_box();

	handle_food(food_manager, debug);

	++frames_alive;

	if (frames_alive > 1000)
	{
		dead = true;
	}

	const sf::Vector2f center = get_center();
	velocity = center - previous_position;
	previous_position = center;
}


void Protozoa::update_springs()
{
	// updates the springs connecting the cells
	for (Spring& spring : m_springs_)
	{
		Cell& cell_A = m_cells_[spring.cell_A_id];
		Cell& cell_B = m_cells_[spring.cell_B_id];
		spring.update(cell_A, cell_B, frames_alive);
	}
}

void Protozoa::update_cells()
{
	// updates each cell in the organism
	for (Cell& cell : m_cells_)
	{
		cell.update(frames_alive);
	}
}

void Protozoa::bound_cells()
{
	// updates each cell in the organism
	for (Cell& cell : m_cells_)
	{
		cell.bound(*m_world_bounds_);
	}
}


void Protozoa::update_bounding_box()
{
	// Calculates the bounding box of the protozoa by finding the outer cells coordinates
	const sf::Vector2f cell0_pos = m_cells_[0].position_;
	const float radius = m_cells_[0].radius;
	float min_x = cell0_pos.x - radius;
	float min_y = cell0_pos.y - radius;
	float max_x = cell0_pos.x + radius;
	float max_y = cell0_pos.y + radius;

	// we check over every cell to see if it beats the current bounds
	for (const Cell& cell : m_cells_) 
	{
		const sf::Vector2f pos = cell.position_;
		min_x = std::min(min_x, pos.x - radius);
		max_x = std::max(max_x, pos.x + radius);
		min_y = std::min(min_y, pos.y - radius);
		max_y = std::max(max_y, pos.y + radius);
	}

	const float width = max_x - min_x;
	const float height = max_y - min_y;

	m_personal_bounds_ = { min_x, min_y, width, height };
}

