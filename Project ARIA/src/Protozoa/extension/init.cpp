#include "../Protozoa.h"

Protozoa::Protozoa(Circle* world_bounds, sf::RenderWindow* window, sf::CircleShape* cell_renderer, const bool init_cells, 
	const GeneticInformation info)
	: GeneticInformation(info), m_window_ptr_(window), m_cell_renderer_ptr_(cell_renderer),
	m_world_bounds_(world_bounds)
{
	if (world_bounds == nullptr || cell_renderer == nullptr)
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
	const std::vector<CellGenetics>& cell_info = get_cell_genetics();

	m_cells_.resize(cell_info.size(), Cell(CellGenetics()));


	const sf::Vector2f rel_center = m_world_bounds_->rand_pos_in_circle();
	const float spawn_radius = 200.f;

	for (const CellGenetics& genetics : cell_info)
	{
		m_cells_[genetics.id] = Cell(genetics);
		m_cells_[genetics.id].position_ = Circle(rel_center, spawn_radius).rand_pos_in_circle();
	}
}


void Protozoa::initialise_springs()
{
	const std::vector<SpringGenetics>& spring_info_vec = get_spring_genetics();
	m_springs_.resize(spring_info_vec.size());

	for (int i = 0; i < m_springs_.size(); ++i)
	{
		m_springs_[i] = Spring(
			spring_info_vec[i].connecting_cell_ids,
			spring_info_vec[i].colors,
			spring_info_vec[i].starting_rest_length,
			spring_info_vec[i].spring_constant,
			spring_info_vec[i].damping_constant
		);
	}
}

