#include "../Protozoa.h"
#include "../../Utils/random.h"
#include <stdexcept> // Required for std::runtime_error

#include "../../Utils/Graphics/spatial_hash_grid.h"

inline static constexpr float spawn_radius = 100.f; // todo move to settings

Protozoa::Protozoa(int id_, Circle* world_bounds, sf::RenderWindow* window, const bool init_cells)
	: id(id_), m_window_(window), m_world_bounds_(world_bounds)
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

	collision_vector.reserve(30); // todo
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


void Protozoa::update(Protozoa_Vector& protozoa_vector, Container& container)
{
	if (m_cells_.empty()) // No computation is needed if there are no cells
		return;

	update_springs();
	update_cells();

	collision_resolution(protozoa_vector, container);

	bound_cells();
	update_bounding_box();

	++frames_alive;

}

void Protozoa::collision_resolution(Protozoa_Vector& protozoa_vector, Container& container)
{
	// for each neighbour cell that overlaps with our bounding box, we add it to our collision vector and then compute
	// all the collisions between the cells in this vector
	collision_vector.clear();

	for (Cell& cell : m_cells_)
	{
		collision_vector.push_back(&cell);
	}

	for (int i = 0; i < container.size; ++i)
	{
		const int idx = container.at(i);

		Protozoa* protozoa = protozoa_vector.at(idx);

		if (protozoa == this)
			continue;

		sf::FloatRect other_bounds = protozoa->get_bounds();
		
		if (!m_personal_bounds_.intersects(other_bounds))
			continue;
	
		for (Cell& cell : protozoa->get_cells())
		{
			collision_vector.push_back(&cell);
		}
	}

	interal_collision_resolution(collision_vector);
}

void Protozoa::interal_collision_resolution(std::vector < Cell* >& cells )
{
	for (Cell* cellA : cells)
	{
		for (Cell* cellB : cells)
		{
			if (cellA == cellB)
				continue;

			const float dist_sq = dist_squared(cellA->position_, cellB->position_);
			const float local_diam = cellA->radius + cellB->radius;

			if (dist_sq > local_diam * local_diam)
				continue;

			// Cells are not the same & they are intersecting
			const float dist = std::sqrt(dist_sq);
			if (dist == 0.0f) // Prevent division by zero
				continue;

			// Compute the overlap
			float overlap = local_diam - dist;

			// Compute the collision normal
			sf::Vector2f collisionNormal = (cellB->position_ - cellA->position_) / dist;

			// Move the cells apart
			cellA->position_ -= collisionNormal * (overlap * 0.5f);
			cellB->position_ += collisionNormal * (overlap * 0.5f);
		}
	}
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
		if (pos.x < min_x) min_x = pos.x - radius;
		if (pos.x > max_x) max_x = pos.x + radius;
		if (pos.y < min_y) min_y = pos.y - radius;
		if (pos.y > max_y) max_y = pos.y + radius;
	}

	const float width = max_x - min_x;
	const float height = max_y - min_y;

	m_personal_bounds_ = { min_x, min_y, width, height };
}

