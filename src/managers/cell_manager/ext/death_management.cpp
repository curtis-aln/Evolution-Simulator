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
	sf::Vector2f vel = bodies_->at(cell->body_id_)->velocity_;
	float speed_sq = vel.lengthSquared();
	float min_speed_sq = statistics_.min_speed * statistics_.min_speed;

	bool is_too_slow = speed_sq < min_speed_sq;

	if (is_too_slow)
	{
		cell->energy -= 0.4f;
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
		
		matter_birth_requests.push_back({cell_id, pos});
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