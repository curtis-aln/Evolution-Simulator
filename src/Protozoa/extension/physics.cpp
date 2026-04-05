#include "../Protozoa.h"
#include "../../Utils/random.h"

#include "../../food_manager.h"

inline static constexpr  size_t cell_positions_container_reserve = 30;
inline static constexpr  size_t food_positions_container_reserve = 30;


Protozoa::Protozoa(const int id_, Circle* world_bounds, sf::RenderWindow* window)
	: m_window_(window), m_world_bounds_(world_bounds), id(id_), GenomeManager(&m_cells_, &m_springs_)
{
	food_positions_nearby.reserve(cell_positions_container_reserve);
	cell_positions_nearby.reserve(food_positions_container_reserve);

	// if no world bounds are provided, we cannot initialise cells as we do not know where to spawn them
	if (world_bounds == nullptr)
	{
		std::cerr << "ERROR: Protozoa was created without world bounds, cannot initialise cells.\n";
		std::cerr << "Protozoa ID: " << id_ << " is window nullptr: " << (window == nullptr) << "\n";
	}
}


void Protozoa::update(FoodManager& food_manager, const bool debug, const float min_speed)
{
	if (m_cells_.empty()) // No computation is needed if there are no cells
		return;

	check_death_conditions(min_speed);

	update_springs();

	update_bounding_box();

	update_cells();

	handle_food(food_manager, debug);

	++frames_alive;

	energy -= energy_decay_rate;
	

	const sf::Vector2f center = get_center();
	velocity = center - previous_position;
	previous_position = center;
}


void Protozoa::check_death_conditions(float min_speed)
{
	float sp = min_speed;
	if (velocity.x * velocity.x + velocity.y * velocity.y < sp * sp)
	{
		dead = true;
	}

	if (energy <= 0)
	{
		dead = true;
	}
}


void Protozoa::update_springs()
{
	// updates the springs connecting the cells
	for (Spring& spring : m_springs_)
	{
		Cell& cell_A = m_cells_[spring.cell_A_id];
		Cell& cell_B = m_cells_[spring.cell_B_id];
		float energy_expendage = spring.update(cell_A, cell_B, frames_alive);
		energy -= energy_expendage;

		if (spring.broken)
		{
			dead = true;
			return;
		}
	}
}

void Protozoa::update_cells()
{
	// updates each cell in the organism
	for (Cell& cell : m_cells_)
	{
		cell.update(frames_alive);

		if (cell_wander_check(cell))
		{
			dead = true;
			return;
		}
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

	m_personal_bounds_ = { {min_x, min_y}, {width, height} };
}

