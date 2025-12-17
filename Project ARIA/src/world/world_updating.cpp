#include "world.h"



void World::update(const bool pause)
{
	check_for_extinction_event();

	iterations_++;
	update_global_cell_vector();
	update_spatial_grid();

	if (toggle_collisions)
	{
		// iterating over each grid cell, we update as per the cell instead of as per the particle
		for (int cell_id = 0; cell_id < cells_x * cells_y; ++cell_id)
		{
			update_grid_cell(cell_id);
		}
	}

	// if our selected protozoa has died we can no longer track it
	if (selected_protozoa_ != nullptr && selected_protozoa_->dead)
	{
		selected_protozoa_ = nullptr;
	}

	if (!pause)
	{
		food_manager_.update();
		update_all_protozoa(food_manager_, debug_mode, min_speed);
	}

	min_speed += 0.00002f;
}


void World::update_grid_cell(const int grid_cell_id)
{
	/* All of the particles in this cell will be updated based on the information of the sorrounding 9 neighbours, this is a more optimized approach */
	int neighbours_size = 0;

	const int cell_index_x = grid_cell_id % cells_x;
	const int cell_index_y = grid_cell_id / cells_x;
	const bool at_border_x = cell_index_x == 0 || cell_index_x == cells_x - 1;
	const bool at_border_y = cell_index_y == 0 || cell_index_y == cells_y - 1;

	// each possible neighbour in the 3x3 area
	update_nearby_container(neighbours_size, cell_index_x - 1, cell_index_y - 1, true, true);
	update_nearby_container(neighbours_size, cell_index_x, cell_index_y - 1, false, true);
	update_nearby_container(neighbours_size, cell_index_x + 1, cell_index_y - 1, true, true);
	update_nearby_container(neighbours_size, cell_index_x - 1, cell_index_y, true, false);
	update_nearby_container(neighbours_size, cell_index_x, cell_index_y, false, false);
	update_nearby_container(neighbours_size, cell_index_x + 1, cell_index_y, true, false);
	update_nearby_container(neighbours_size, cell_index_x - 1, cell_index_y + 1, true, true);
	update_nearby_container(neighbours_size, cell_index_x, cell_index_y + 1, false, true);
	update_nearby_container(neighbours_size, cell_index_x + 1, cell_index_y + 1, true, true);

	// updating the particles
	const auto& cell_contents = spatial_hash_grid_.grid[grid_cell_id];
	const uint8_t cell_size = spatial_hash_grid_.objects_count[grid_cell_id];

	//#pragma omp parallel for
	for (uint8_t idx = 0; idx < cell_size; ++idx)
	{
		update_protozoa_cell(cell_contents[idx], neighbours_size);
	}
}

void World::update_protozoa_cell(const int protozoa_cell_index, const int neighbours_size) const
{
	Cell* cell = global_cell_vector_[protozoa_cell_index];

	for (uint32_t i{ 0 }; i < neighbours_size; ++i)
	{
		Cell* other_cell = global_cell_vector_[nearby_ids[i]];
		const sf::Vector2f other_pos = other_cell->position_;

		const float dist_sq = dist_squared(cell->position_, other_pos);
		const float local_diam = cell->radius * 2.f;

		if (dist_sq > local_diam * local_diam)
			continue;

		// Cells are not the same & they are intersecting
		const float dist = std::sqrt(dist_sq);
		if (dist == 0.0f) // Prevent division by zero
			continue;

		// Compute the overlap
		float overlap = local_diam - dist;

		// Compute the collision normal
		sf::Vector2f collisionNormal = (cell->position_ - other_pos) / dist;

		// Move the cells apart
		cell->position_ += collisionNormal * (overlap * 0.5f);
		other_cell->position_ -= collisionNormal * (overlap * 0.5f);

	}
}

void World::update_nearby_container(int& neighbours_size,
	int32_t neighbour_index_x, int32_t neighbour_index_y,
	const bool check_x = true,
	const bool check_y = true)
{
	// Fast modulo for positive and negative numbers
	if (check_x)
	{
		neighbour_index_x = neighbour_index_x >= 0 ?
			(neighbour_index_x < cells_x ? neighbour_index_x : neighbour_index_x - cells_x) :
			(neighbour_index_x + cells_x);
	}

	if (check_y)
	{
		neighbour_index_y = neighbour_index_y >= 0 ?
			(neighbour_index_y < cells_y ? neighbour_index_y : neighbour_index_y - cells_y) :
			(neighbour_index_y + cells_y);
	}

	// fetching data for copying
	const uint32_t neighbour_index = neighbour_index_y * cells_x + neighbour_index_x;
	const auto& contents = spatial_hash_grid_.grid[neighbour_index];
	const auto size = spatial_hash_grid_.objects_count[neighbour_index];

	// adding the neighbour data to the array
//#pragma omp parallel for
	for (uint8_t idx = 0; idx < size; ++idx)
	{
		if (neighbours_size >= nearby_ids.size())
		{
			std::cout << "[WARNING]: Nearby IDs buffer overflow, increase its size to avoid data loss.\n";
			break;
		}

		nearby_ids[neighbours_size++] = contents[idx];
	}
}


void World::update_spatial_grid()
{
	spatial_hash_grid_.clear();
	int idx = 0;
	for (Cell* cell : global_cell_vector_)
	{
		cell->bound(world_circular_bounds_);
		spatial_hash_grid_.add_object(cell->position_.x, cell->position_.y, idx++);
	}
}


void World::update_position_container()
{
	outer_color_data_.clear();
	inner_color_data_.clear();
	position_data_.clear();

	food_manager_.render();

	for (Protozoa* protozoa : all_protozoa_)
	{
		for (Cell& cell : protozoa->get_cells())
		{
			outer_color_data_.push_back(protozoa->cell_outer_color);
			inner_color_data_.push_back(protozoa->cell_inner_color);
			position_data_.push_back(cell.position_);
		}
	}
}