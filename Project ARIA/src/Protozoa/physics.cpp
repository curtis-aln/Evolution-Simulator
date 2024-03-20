#include "Protozoa.h"
#include "../Utils/random.h"
#include <stdexcept> // Required for std::runtime_error



Protozoa::Protozoa(const sf::Rect<float>& bounds, sf::RenderWindow* window, sf::CircleShape* cell_renderer)
	: m_window(window), m_cell_renderer(cell_renderer), m_world_bounds(bounds)
{
	initialise_cells();
}


void Protozoa::initialise_cells()
{
	// when creating the cells for this organism we need to first define the cell count, and the cell spawn radius which is not too important
	// we create the cells in two steps: personal information and relational information. personal is info only the cell uses on itself. relational
	// information is who the cell's parent / children are
	const unsigned cell_count = RandomDist::randRange(max_cells/2, max_cells);
	constexpr float spawn_rad = 75.f;

	const sf::Vector2f rel_center = RandomDist::randPosInRect(m_world_bounds);
	const sf::Rect spawn_range = { rel_center.x - spawn_rad, rel_center.y - spawn_rad,
		spawn_rad * 2, spawn_rad * 2 };

	m_cells.resize(cell_count, Cell());

	for (unsigned i = 0; i < cell_count; ++i)
	{
		m_cells[i]._relID = i;
		m_cells[i].set_position(RandomDist::randPosInRect(spawn_range));
	}

	for (unsigned i = 0; i < cell_count; ++i)
		set_parent_cell(&m_cells[i]);
}

void Protozoa::set_parent_cell(Cell* cell)
{
	// choosing a random cell that passes the following conditions:
	// 1. is not `cell`
	// 2. `cell`is not already a child of parent cell
	// 3. `cell` is not the parent of parent cell
	const unsigned int total_cells = m_cells.size();


	for (int i = 0; i < 50; ++i)
	{
		Cell* parent_cell = &m_cells[RandomDist::randRange(0u, total_cells - 1)];

		if (!create_cellular_connection(parent_cell, cell))
			continue;

		return;
	}
	return; //todo
	throw std::runtime_error("Failed to make a connection");
}

bool Protozoa::create_cellular_connection(Cell* parent_cell, Cell* child_cell)
{
	if (parent_cell != child_cell || !parent_cell->is_child(child_cell->_relID) || !parent_cell->is_parent(child_cell))
		return false;

	parent_cell->add_child(child_cell->_relID);
	child_cell->set_parent(parent_cell);
	return true;
}



void Protozoa::update()
{
	update_bounds();
}


void Protozoa::update_bounds()
{
	sf::Vector2f cell0_pos = m_cells[0].get_position();
	float minX = cell0_pos.x - m_cells[0].get_radius();
	float minY = cell0_pos.y - m_cells[0].get_radius();
	float maxX = cell0_pos.x + m_cells[0].get_radius();
	float maxY = cell0_pos.y + m_cells[0].get_radius();

	for (const Cell& cell : m_cells) 
	{
		sf::Vector2f pos = cell.get_position();
		if (pos.x < minX) minX = pos.x - cell.get_radius();
		if (pos.x > maxX) maxX = pos.x + cell.get_radius();
		if (pos.y < minY) minY = pos.y - cell.get_radius();
		if (pos.y > maxY) maxY = pos.y + cell.get_radius();
	}

	const float width = maxX - minX;
	const float height = maxY - minY;

	m_personal_bounds = { minX, minY, width, height };
}


bool Protozoa::is_hovered_on(const sf::Vector2f mousePosition) const
{
	return m_personal_bounds.contains(mousePosition);
}

void Protozoa::set_debug_mode(const bool mode)
{
	debug_mode = mode;
}