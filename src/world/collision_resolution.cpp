#include "world.h"


void World::resolve_collisions()
{
	if (!toggle_collisions)
		return;

	for (int cell_id = 0; cell_id < spatial_hash_grid_.CellsX * spatial_hash_grid_.CellsY; ++cell_id)
	{
		update_cells_in_grid_cell(cell_id);
	}
}


void World::update_cells_in_grid_cell(const int grid_cell_id)
{
	// if the grid cell is empty, dont bother
	if (spatial_hash_grid_.cell_capacities[grid_cell_id] == 0)
		return;

	int neighbours_size = 0;

	const int cell_index_x = grid_cell_id % spatial_hash_grid_.CellsX;
	const int cell_index_y = grid_cell_id / spatial_hash_grid_.CellsX;

	// Current cell
	update_nearby_container(neighbours_size, cell_index_x, cell_index_y);
	// Right
	update_nearby_container(neighbours_size, cell_index_x + 1, cell_index_y);
	// Bottom-left
	update_nearby_container(neighbours_size, cell_index_x - 1, cell_index_y + 1);
	// Bottom
	update_nearby_container(neighbours_size, cell_index_x, cell_index_y + 1);
	// Bottom-right
	update_nearby_container(neighbours_size, cell_index_x + 1, cell_index_y + 1);

	const auto& cell_contents = spatial_hash_grid_.grid[grid_cell_id];
	const uint8_t cell_size = spatial_hash_grid_.cell_capacities[grid_cell_id];

	for (uint8_t idx = 0; idx < cell_size; ++idx)
	{
		update_protozoa_cell(cell_contents[idx], neighbours_size);
	}
}


void World::update_nearby_container(int& neighbours_size, int32_t neighbour_index_x, int32_t neighbour_index_y)
{
	// Out of bounds check, no wrapping needed
	if (neighbour_index_x < 0 || neighbour_index_x >= static_cast<int>(spatial_hash_grid_.CellsX) ||
		neighbour_index_y < 0 || neighbour_index_y >= static_cast<int>(spatial_hash_grid_.CellsY))
		return;

	const uint32_t neighbour_index = neighbour_index_y * spatial_hash_grid_.CellsX + neighbour_index_x;
	const auto& contents = spatial_hash_grid_.grid[neighbour_index];
	const uint8_t size = spatial_hash_grid_.cell_capacities[neighbour_index];

	for (uint8_t idx = 0; idx < size; ++idx)
	{
		nearby_ids[neighbours_size++] = contents[idx];
	}
}

void World::update_protozoa_cell(const int protozoa_cell_index, const int neighbours_size)
{
	sf::Vector2f position_a = position_data_[protozoa_cell_index];

	for (uint32_t i{ 0 }; i < neighbours_size; ++i)
	{
		// check if the cells are the same
		if (protozoa_cell_index == nearby_ids[i])
			continue;

		const sf::Vector2f position_b = position_data_[nearby_ids[i]];

		const float dist_sq = (position_a - position_b).lengthSquared();
		const float local_diam = radius_data_[protozoa_cell_index] + radius_data_[nearby_ids[i]];

		if (dist_sq > local_diam * local_diam)
			continue;

		// Cells are not the same & they are intersecting
		const float dist = std::sqrt(dist_sq);
		if (dist == 0.0f) // Prevent division by zero
			continue;

		// Compute the overlap
		float overlap = local_diam - dist;

		// Compute the collision normal
		sf::Vector2f collisionNormal = (position_a - position_b) / dist;

		// Move the cells apart
		collision_resolutions[protozoa_cell_index] = collisionNormal * (overlap * 0.5f);
		collision_resolutions[nearby_ids[i]] = -collisionNormal * (overlap * 0.5f);

		Cell* cell_a = cell_pointers_[protozoa_cell_index];
		Cell* cell_b = cell_pointers_[nearby_ids[i]];
		cell_a->colliding_with_ = cell_b->position_;
		cell_b->colliding_with_ = cell_a->position_;
		cell_a->collision_ids = { protozoa_cell_index, cell_b->id };
		cell_b->collision_ids = { protozoa_cell_index, cell_b->id };
	}
}
