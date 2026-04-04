#include "world.h"



void World::update()
{
	min_speed += delta_min_speed;
	check_for_extinction_event(world_circular_bounds_);

	iterations_++;
	update_position_container();

	if (track_statistics)
		update_statistics();

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

	food_manager_.update();
	update_all_protozoa(food_manager_, debug_mode, min_speed, track_statistics);
	
}


void World::update_grid_cell(const int grid_cell_id)
{
	int neighbours_size = 0;

	const int cell_index_x = grid_cell_id % cells_x;
	const int cell_index_y = grid_cell_id / cells_x;

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
	const uint8_t cell_size = spatial_hash_grid_.objects_count[grid_cell_id];

	for (uint8_t idx = 0; idx < cell_size; ++idx)
	{
		update_protozoa_cell(cell_contents[idx], neighbours_size);
	}
}

void World::update_protozoa_cell(const int protozoa_cell_index, const int neighbours_size)
{
	sf::Vector2f position_a = position_data_[protozoa_cell_index];

	for (uint32_t i{ 0 }; i < neighbours_size; ++i)
	{
		const sf::Vector2f position_b = position_data_[nearby_ids[i]];

		const float dist_sq = (position_a - position_b).lengthSquared();
		const float local_diam = CellGenome::radius * 2.f;

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
	}
}

void World::update_nearby_container(int& neighbours_size,
    int32_t neighbour_index_x, int32_t neighbour_index_y)
{
    // Out of bounds check, no wrapping needed
    if (neighbour_index_x < 0 || neighbour_index_x >= cells_x ||
        neighbour_index_y < 0 || neighbour_index_y >= cells_y)
        return;

    const uint32_t neighbour_index = neighbour_index_y * cells_x + neighbour_index_x;
    const auto& contents = spatial_hash_grid_.grid[neighbour_index];
    const uint8_t size = spatial_hash_grid_.objects_count[neighbour_index];

    for (uint8_t idx = 0; idx < size; ++idx)
    {
        nearby_ids[neighbours_size++] = contents[idx];
    }
}


void World::update_position_container()
{
	outer_color_data_.clear();
	inner_color_data_.clear();
	position_data_.clear();

	spatial_hash_grid_.clear();

	int idx = 0;
	for (Protozoa* protozoa : all_protozoa_)
	{
		for (Cell& cell : protozoa->get_cells())
		{
			cell.bound(world_circular_bounds_);
			outer_color_data_.push_back(cell.cell_outer_color);
			inner_color_data_.push_back(cell.cell_inner_color);
			position_data_.push_back(cell.position_);

			spatial_hash_grid_.add_object(cell.position_.x, cell.position_.y, idx++);
		}
	}

	collision_resolutions.resize(position_data_.size(), { 0.f, 0.f });
}


void World::update_statistics()
{
	++frames_since_last_gen_change;

	average_generation_ = get_average_generation();

	if (int(tracked_generation_) != int(average_generation_))
	{
		frames_per_generation_ = (tracked_generation_ != 0.f)
			? (iterations_ - frames_since_last_gen_change) : iterations_;
		frames_since_last_gen_change = 0;
		tracked_generation_ = average_generation_;
	}

	float protozoa_count = static_cast<float>(all_protozoa_.size());
	if (protozoa_count == 0)
		return;

	average_cells_per_protozoa_ = 0.f;
	average_offspring_count_ = 0.f;
	average_mutation_rate_ = 0.f;
	average_mutation_range_ = 0.f;

	// Calculating averages for cells per protozoa, offspring count, mutation rate, and mutation range across all protozoa
	int cell_count = 0;
	for (Protozoa* protozoa : all_protozoa_)
	{
		for (Cell& cell : protozoa->get_cells())
		{
			average_mutation_rate_ += cell.mutation_rate;
			average_mutation_range_ += cell.mutation_range;

			cell_count++;
		}
		average_cells_per_protozoa_ += protozoa->get_cells().size();
		average_offspring_count_ += protozoa->offspring_count;
	}

	average_cells_per_protozoa_ /= protozoa_count;
	average_offspring_count_ /= protozoa_count;

	average_mutation_rate_ /= cell_count;
	average_mutation_range_ /= cell_count;

	// Updating death and birth rates per hundred frames
	if (iterations_ % survival_rate_window_size_ == 0)
	{
		deaths_per_hundered_frames_ = static_cast<float>(deaths_this_window_);
		births_per_hundered_frames_ = static_cast<float>(births_this_window_);

		// Reset window
		deaths_this_window_ = 0;
		births_this_window_ = 0;
	}
}

