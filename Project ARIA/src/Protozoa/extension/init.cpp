#include "../Protozoa.h"

Protozoa::Protozoa(Circle* world_bounds, sf::RenderWindow* window, sf::CircleShape* cell_renderer, const bool init_cells)
	: m_window_ptr_(window), m_cell_renderer_ptr_(cell_renderer), m_world_bounds_(world_bounds),
	m_info_font_(window, 10, "src/Utils/fonts/Roboto-Regular.ttf")
{
	if (world_bounds == nullptr || cell_renderer == nullptr)
	{
		return;
	}

	if (init_cells)
	{
		initialise_cells();
	}

	initialise_springs();
}


void Protozoa::initialise_cells()
{
	// preparing the cell container

	const sf::Vector2f rel_center = m_world_bounds_->rand_pos_in_circle();
	m_cells_.reserve(max_cells);

	// creating the first cell. it will have 2 children. those children will have a 50% chance of having another child
	m_cells_.emplace_back(nullptr, 0);
	create_children_for_cell(m_cells_[0], 0.25f, 0, true);

	for (Cell& cell : m_cells_)
	{
		constexpr float spawn_rad = 75.f;
		cell.set_position(create_cell_position(rel_center, spawn_rad));
	}
}


sf::Vector2f Protozoa::create_cell_position(const sf::Vector2f relative_center, const float spawn_range)
{
	return Circle(relative_center, spawn_range).rand_pos_in_circle();
}


void Protozoa::create_children_for_cell(Cell& cell, const float probability, const int depth, const bool is_parent = false)
{
	if (depth >= 3)
		return;

	if (is_parent == true)
	{
		constexpr unsigned initial_cell_children = 2;
		for (int i = 0; i < initial_cell_children; ++i)
		{
			create_cell(&cell, probability, depth);
		}
	}

	else
	{
		constexpr int max_offspring = 3;

		for (int i = 0; i < max_offspring; ++i)
		{
			if (Random::rand01_float() < probability)
			{
				create_cell(&cell, probability, depth);
			}
		}
	}
}


void Protozoa::create_cell(Cell* parent, const float probability, int depth)
{
	const int id = m_cells_.size();
	m_cells_.emplace_back(nullptr, id);
	create_cellular_connection(parent, &m_cells_[id]);
	create_children_for_cell(m_cells_[id], probability, ++depth);
}


void Protozoa::initialise_springs()
{
	// iterate over each cell, if cell has children then for each child, make a spring between them both if they do not already have one. 
	for (Cell& cell : m_cells_)
	{
		for (const int child_id : cell.get_children_ids())
		{
			if (!does_spring_exist_between(cell.rel_id, child_id))
			{
				m_springs_.emplace_back(cell.rel_id, child_id);
			}
		}
	}
}

bool Protozoa::does_spring_exist_between(const int cellA_id, const int cellB_id) const
{
	// todo std anyranges of
	for (const Spring& spring : m_springs_)
	{
		if ((spring.m_cellA_id == cellA_id && spring.m_cellB_id == cellB_id) || (spring.m_cellA_id == cellB_id && spring.m_cellB_id == cellA_id))
		{
			return true;
		}
	}
	return false;
}


void Protozoa::create_cellular_connection(Cell* parent_cell, Cell* child_cell)
{
	parent_cell->add_child(child_cell->rel_id);
	child_cell->set_parent(parent_cell);
}


