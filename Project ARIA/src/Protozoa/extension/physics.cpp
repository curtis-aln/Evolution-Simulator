#include "../Protozoa.h"
#include "../../Utils/random.h"

#include "../../Utils/Graphics/spatial_hash_grid.h"
#include "../../food_manager.h"

inline static constexpr  size_t cell_positions_container_reserve = 30;
inline static constexpr  size_t food_positions_container_reserve = 30;


Protozoa::Protozoa(const int id_, Circle* world_bounds, sf::RenderWindow* window, const bool init_cells)
	: m_window_(window), m_world_bounds_(world_bounds), id(id_)
{

	food_positions_nearby.reserve(cell_positions_container_reserve);
	cell_positions_nearby.reserve(food_positions_container_reserve);

	// if no world bounds are provided, we cannot initialise cells as we do not know where to spawn them
	if (world_bounds == nullptr)
	{
		// raise error
		std::cerr << "ERROR: Protozoa created without world bounds, cannot initialise cells.\n";
	}

	if (!init_cells)
		return;

	generate_random();
}


void Protozoa::generate_random()
{
	auto preset = Random::rand_val_in_vector(presets);
	load_preset(preset);
}


void Protozoa::update(FoodManager& food_manager, const bool debug)
{
	if (m_cells_.empty()) // No computation is needed if there are no cells
		return;

	update_springs();
	update_cells();

	update_bounding_box();

	handle_food(food_manager, debug);

	++frames_alive;

	energy -= 0.15;
	if (energy <= 0)
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
		spring.update(cell_A, cell_B, frames_alive, get_spring_gene(spring.id));
	}
}

void Protozoa::update_cells()
{
	// updates each cell in the organism
	for (Cell& cell : m_cells_)
	{
		cell.update(frames_alive, get_cell_gene(cell.id));
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

