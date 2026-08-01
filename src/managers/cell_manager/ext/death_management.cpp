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
}

void CellManager::remove_cell(Cell* cell)
{
    if (cell == nullptr) return;
    register_death_stat(cell->frames_alive_, cell->offspring_count > 0);

    if (cell->pending_parent_id != -1)
    {
        if (Cell* parent = all_cells_.at(cell->pending_parent_id))
        {
            if (parent->offspring_index == static_cast<int32_t>(cell->id_))
            {
                parent->offspring_index = -1;
                parent->connection_index = -1;
                parent->spring_to_copy_index = -1;
                parent->frames_since_offspring_pending_ = 0;
            }
        }
    }

    all_cells_.remove(cell);
    bodies_->remove(cell->body_id_);
}