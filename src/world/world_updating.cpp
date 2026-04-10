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
	update_cell_collisions();

	food_manager_.update();
	resolve_food_interactions();
	
	update_all_protozoa(food_manager_, debug_mode, min_speed, track_statistics, toggle_collisions);
	
	
}


void World::update_position_container()
{
	spatial_hash_grid_.clear();

	// fill the collision resolution vector with zeroes, we will fill it with new resolutions in the collision detection phase and then apply them to the cells in the update phase
	std::fill(collision_resolutions.begin(), collision_resolutions.end(), sf::Vector2f{ 0.f, 0.f });

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
			cell.colliding_with_ = cell.position_;
			cell.collision_resolution_vector_ = { 0.f, 0.f };
			//c//ell.collision_ids = { -1, -1 };

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

	// Peak population
	const int p_count = static_cast<int>(all_protozoa_.size());
	if (p_count > peak_protozoa_ever_) peak_protozoa_ever_ = p_count;

	// Per-organism aggregates
	float total_energy = 0.f;
	float total_springs = 0.f;
	float sum_amp = 0.f, sum_sq = 0.f;
	int   cell_div_count = 0;

	for (Protozoa* p : all_protozoa_)
	{
		total_energy += p->get_energy();
		total_springs += static_cast<float>(p->get_springs().size());
		if (p->offspring_count > most_offspring_ever_)
			most_offspring_ever_ = p->offspring_count;
		for (const Cell& c : p->get_cells())
		{
			sum_amp += c.amplitude;
			sum_sq += c.amplitude * c.amplitude;
			++cell_div_count;
			if (c.generation > highest_generation_ever_)
				highest_generation_ever_ = c.generation;
		}
	}

	if (protozoa_count > 0)
	{
		average_energy_ = total_energy / protozoa_count;
		average_spring_count_ = total_springs / protozoa_count;
		energy_efficiency_ = average_energy_ / 300.f; // ratio vs starting energy
	}
	if (cell_div_count > 0)
	{
		const float mean_amp = sum_amp / cell_div_count;
		genetic_diversity_ = (sum_sq / cell_div_count) - (mean_amp * mean_amp);
	}
}

