#include "world.h"



void World::update()
{
	min_speed += delta_min_speed;
	check_for_extinction_event(world_circular_bounds_);

	iterations_++;
	update_position_container();

	if (track_statistics)
		update_statistics();

	// if our selected protozoa has died we can no longer track it
	if (selected_protozoa_ != nullptr && !selected_protozoa_->is_alive())
	{
		selected_protozoa_ = nullptr;
	}

	resolve_collisions();

	food_manager_.update();
	resolve_food_interactions();
	update_all_protozoa(food_manager_, debug_mode, min_speed, track_statistics);
	
	
}


void World::update_position_container()
{
	spatial_hash_grid_.clear();

	int idx = 0;
	for (Protozoa* protozoa : all_protozoa_)
	{
		for (Cell& cell : protozoa->get_cells())
		{
			cell.bound(world_circular_bounds_);
			outer_color_data_[idx] = cell.cell_outer_color;
			inner_color_data_[idx] = cell.cell_inner_color;
			position_data_[idx] = cell.position_;
			radius_data_[idx] = cell.radius;
			cell_pointers_[idx] = &cell;

			spatial_hash_grid_.add_object(cell.position_.x, cell.position_.y, idx);
			idx++;
		}
	}

	entity_count = idx;
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

