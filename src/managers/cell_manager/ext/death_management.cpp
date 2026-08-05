#include "../cell_manager.h"

// The only case in which a cell is removed from the world:
// integrity is zero

void CellManager::handle_death()
{
	for (Cell* cell : all_cells_)
	{
		if (!cell->should_remove() || !bodies_->is_obj_active(cell->body_id_))
			continue;
		remove_cell(cell);
	}

	for (CellMatter* matter : all_cell_matter_)
	{
		if (!matter->dead || !bodies_->is_obj_active(matter->body_id_))
			continue;
		remove_cell_matter(matter);
	}
}

void CellManager::remove_cell(Cell* cell)
{
    if (cell == nullptr) return;
    register_death_stat(cell->internal_clock_, cell->offspring_count > 0);

    all_cells_.remove(cell);
    bodies_->remove(cell->body_id_);
}

void CellManager::remove_cell_matter(CellMatter* matter)
{
	if (matter == nullptr) return;
	all_cell_matter_.remove(matter);
	bodies_->remove(matter->body_id_);
}