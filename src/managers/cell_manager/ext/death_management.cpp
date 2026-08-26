#include "../cell_manager.h"

// The only case in which a cell is removed from the world:
// integrity is zero

void CellManager::collect_matter_death_requests()
{
	for (CellMatter* matter : all_cell_matter_)
	{
		if (!matter->dead || !bodies_->is_obj_active(matter->body_id_))
			continue;

		matter_death_requests_.push_back(matter->id_);
	}
}

void CellManager::collect_cell_death_requests()
{
	for (Cell* cell : all_cells_)
	{
		if (!cell->is_alive() || !bodies_->is_obj_active(cell->body_id_))
			cell_death_requests_.push_back(cell->id_);
	}
}

void CellManager::speed_tax_cell(Cell* cell)
{
	float speed = bodies_->at(cell->body_id_)->velocity_.length();
	if (speed < statistics_.min_speed)
	{
		float deficit_ratio = 1.f - (speed / statistics_.min_speed); // 0 at threshold, 1 at rest
		cell->change_energy(speed_energy_tax * deficit_ratio);
	}
}

void CellManager::remove_cell(cell_idx cell_id)
{
    Cell* cell = all_cells_.at(cell_id);
    if (cell == nullptr) return;
    register_death_stat(cell->internal_clock_, cell->offspring_count > 0);

	cell->kill();

    all_cells_.remove(cell);
    bodies_->remove(cell->body_id_);
}

void CellManager::remove_cell_matter(cell_idx matter_id)
{
	CellMatter* matter = all_cell_matter_.at(matter_id);
	if (matter == nullptr) return;
	all_cell_matter_.remove(matter);
	bodies_->remove(matter->body_id_);
}

void CellManager::apply_cell_death_requests()
{
	for (cell_idx cell_id : cell_death_requests_)
	{
		sf::Vector2f pos = get_cell_pos(cell_id);
		Cell* cell = all_cells_.at(cell_id);
		matter_birth_requests.push_back({pos, cell->get_inner_color(), cell->get_outer_color()});
		remove_cell(cell_id);
	}
	cell_death_requests_.clear();
}

void CellManager::apply_matter_death_requests()
{
	for (cell_idx matter_id : matter_death_requests_)
	{
		remove_cell_matter(matter_id);
	}
	matter_death_requests_.clear();
}