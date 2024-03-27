#include "Protozoa.h"
#include "../Utils/random.h"
#include <stdexcept> // Required for std::runtime_error



void Protozoa::update()
{
	if (m_cells_.size() == 0)
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
	sf::Vector2f cell0_pos = m_cells_[0].get_position();
	float minX = cell0_pos.x - m_cells_[0].get_radius();
	float minY = cell0_pos.y - m_cells_[0].get_radius();
	float maxX = cell0_pos.x + m_cells_[0].get_radius();
	float maxY = cell0_pos.y + m_cells_[0].get_radius();

	for (const Cell& cell : m_cells_) 
	{
		const sf::Vector2f pos = cell.get_position();
		if (pos.x < minX) minX = pos.x - cell.get_radius();
		if (pos.x > maxX) maxX = pos.x + cell.get_radius();
		if (pos.y < minY) minY = pos.y - cell.get_radius();
		if (pos.y > maxY) maxY = pos.y + cell.get_radius();
	}

	const float width = maxX - minX;
	const float height = maxY - minY;

	m_personal_bounds_ = { minX, minY, width, height };
}


bool Protozoa::is_hovered_on(const sf::Vector2f mousePosition) const
{
	return m_personal_bounds_.contains(mousePosition);
}

void Protozoa::set_debug_mode(const bool mode)
{
	debug_mode_ = mode;
}