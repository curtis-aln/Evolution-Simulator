#include "../Protozoa.h"

inline static constexpr float spawn_radius = 200.f; // todo move to settings

Protozoa::Protozoa(Circle* world_bounds, sf::RenderWindow* window, const bool init_cells)
	: m_window_ptr_(window), m_world_bounds_(world_bounds)
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
	const auto& spring_genes = m_genes_.spring_genes;
	m_springs_.reserve(spring_genes.size());

	for (SpringGene& gene : m_genes_.spring_genes)
	{
		Spring spring{ {gene.cell_A_id, gene.cell_B_id}, {gene.inner_color, gene.outer_color},
			gene.rest_length, gene.spring_constant, gene.damping };
		m_springs_.emplace_back(spring);
	}
}

