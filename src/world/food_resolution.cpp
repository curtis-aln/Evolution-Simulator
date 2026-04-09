#include "world.h"


void World::resolve_food_interactions()
{
	for (int cell_id = 0; cell_id < cells_x * cells_y; ++cell_id)
	{
		resolve_food_grid_cell(cell_id);
	}
}

void World::resolve_food_grid_cell(const int cell_id)
{
	SimpleSpatialGrid* food_grid = get_food_spatial_grid();

	// if the cell grid cell is empty, dont bother
	if (spatial_hash_grid_.cell_capacities[cell_id] == 0)
		return;

	food_grid->find_from_index(cell_id, &nearby_food);

	// now we get all the cells in the same cell grid
	const auto& cell_contents = spatial_hash_grid_.grid[cell_id];
	const int cell_size = spatial_hash_grid_.cell_capacities[cell_id];

	for (int idx = 0; idx < cell_size; ++idx)
	{
		Cell* cell = cell_pointers_[cell_contents[idx]];

		cell->handle_nearby_food(nearby_food, food_manager_);
	}
}
