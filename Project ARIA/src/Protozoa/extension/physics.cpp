#include "../Protozoa.h"
#include "../../Utils/random.h"
#include <stdexcept> // Required for std::runtime_error


void Protozoa::update()
{
	if (m_cells_.empty()) // No computation is needed if there are no cells
		return;

	update_bounds();

	update_springs();
	update_cells();

	++frames_alive;

}

void Protozoa::update_springs()
{
	// updates the springs connecting the cells
	for (Spring& spring : m_springs_)
	{
		spring.update(m_cells_[spring.connection.first], m_cells_[spring.connection.second]);
	}
}

void Protozoa::update_cells()
{
	// updates each cell in the organism
	for (Cell& cell : m_cells_)
	{
		cell.update(m_cells_);
		cell.bound(*m_world_bounds_);
	}
}


void Protozoa::update_bounds()
{
	// Calculates the bounding box of the protozoa by finding the outer cells coordinates
	const sf::Vector2f cell0_pos = m_cells_[0].position_;
	const float radius = m_cells_[0].get_radius();
	float min_x = cell0_pos.x - radius;
	float min_y = cell0_pos.y - radius;
	float max_x = cell0_pos.x + radius;
	float max_y = cell0_pos.y + radius;

	// we check over every cell to see if it beats the current bounds
	for (const Cell& cell : m_cells_) 
	{
		const sf::Vector2f pos = cell.position_;
		if (pos.x < min_x) min_x = pos.x - radius;
		if (pos.x > max_x) max_x = pos.x + radius;
		if (pos.y < min_y) min_y = pos.y - radius;
		if (pos.y > max_y) max_y = pos.y + radius;
	}

	const float width = max_x - min_x;
	const float height = max_y - min_y;

	m_personal_bounds_ = { min_x, min_y, width, height };
}

