#include "../Protozoa.h"
#include "../../Utils/random.h"
#include <stdexcept> // Required for std::runtime_error



void Protozoa::update()
{
	if (m_cells_.empty())
		return;

	update_bounds();

	update_springs();
	update_cells();

	++frames_alive;

}

void Protozoa::update_springs()
{
	for (Spring& spring : m_springs_)
	{
		spring.update(m_cells_[spring.m_cellA_id], m_cells_[spring.m_cellB_id]);
	}
}

void Protozoa::update_cells()
{
	for (Cell& cell : m_cells_)
	{
		cell.update(m_cells_);
		cell.bound(*m_world_bounds_);
	}
}


void Protozoa::update_bounds()
{
	const sf::Vector2f cell0_pos = m_cells_[0].get_position();
	float min_x = cell0_pos.x - m_cells_[0].get_radius();
	float min_y = cell0_pos.y - m_cells_[0].get_radius();
	float max_x = cell0_pos.x + m_cells_[0].get_radius();
	float max_y = cell0_pos.y + m_cells_[0].get_radius();

	for (const Cell& cell : m_cells_) 
	{
		const sf::Vector2f pos = cell.get_position();
		if (pos.x < min_x) min_x = pos.x - cell.get_radius();
		if (pos.x > max_x) max_x = pos.x + cell.get_radius();
		if (pos.y < min_y) min_y = pos.y - cell.get_radius();
		if (pos.y > max_y) max_y = pos.y + cell.get_radius();
	}

	const float width = max_x - min_x;
	const float height = max_y - min_y;

	m_personal_bounds_ = { min_x, min_y, width, height };
}

