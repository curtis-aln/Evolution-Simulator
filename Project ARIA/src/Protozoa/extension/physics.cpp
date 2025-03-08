#include "../Protozoa.h"
#include "../../Utils/random.h"
#include <stdexcept> // Required for std::runtime_error

inline static constexpr float spawn_radius = 200.f; // todo move to settings

Protozoa::Protozoa(Circle* world_bounds, sf::RenderWindow* window, const bool init_cells)
	: m_window_(window), m_world_bounds_(world_bounds)
{
	if (world_bounds == nullptr)
	{
		return;
	}

	if (init_cells)
	{
		initialise_cells();
		initialise_springs();
	}
}

void Protozoa::initialise_cells()
{
	// cells are spawned in clusters around an origin
	const Circle protozoa_area = { m_world_bounds_->rand_pos(), spawn_radius };

	m_cells_.reserve(m_genes_.cell_count);
	for (CellGene& gene : m_genes_.cell_genes) // i literally think we can remove the gene class
	{
		Cell cell = { gene };
		cell.position_ = protozoa_area.rand_pos();

		m_cells_.emplace_back(cell);
	}
}

void Protozoa::initialise_springs()
{
	m_springs_.reserve(m_genes_.spring_genes.size());

	for (SpringGene& gene : m_genes_.spring_genes)
	{
		m_springs_.emplace_back(gene);
	}
}


void Protozoa::update()
{
	if (m_cells_.empty()) // No computation is needed if there are no cells
		return;

	update_bounding_box();

	update_springs();
	update_cells();

	++frames_alive;

}

void Protozoa::update_springs()
{
	// updates the springs connecting the cells
	for (Spring& spring : m_springs_)
	{
		Cell& cell_A = m_cells_[spring.cell_A_id];
		Cell& cell_B = m_cells_[spring.cell_B_id];
		spring.update(cell_A, cell_B);
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
		if (pos.x < min_x) min_x = pos.x - radius;
		if (pos.x > max_x) max_x = pos.x + radius;
		if (pos.y < min_y) min_y = pos.y - radius;
		if (pos.y > max_y) max_y = pos.y + radius;
	}

	const float width = max_x - min_x;
	const float height = max_y - min_y;

	m_personal_bounds_ = { min_x, min_y, width, height };
}

