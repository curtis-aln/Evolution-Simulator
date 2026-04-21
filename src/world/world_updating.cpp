#include <algorithm>

#include "world.h"



void World::update()
{
	frame_rate_smoothing_.update_frame_rate();
	statistics_.updating_fps = frame_rate_smoothing_.get_average_frame_rate();
	toggles.min_speed += toggles.delta_min_speed;
	check_for_extinction_event(world_circular_bounds_);

	statistics_.iterations_++;
	update_position_container();

	if (toggles.track_statistics)
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
	
	update_all_protozoa(food_manager_, toggles.debug_mode, toggles.min_speed, toggles.track_statistics, toggles.toggle_collisions);
	
	
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
			render_data_.outer_colors[idx] = cell.cell_outer_color;
			render_data_.inner_colors[idx] = cell.cell_inner_color;
			render_data_.positions_x[idx] = cell.position_.x;
			render_data_.positions_y[idx] = cell.position_.y;
			render_data_.radii[idx] = cell.radius;
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
	++statistics_.frames_since_last_gen_change;

	statistics_.average_generation = get_average_generation();

	if (int(tracked_generation_) != int(statistics_.average_generation))
	{
		statistics_.frames_per_generation = (tracked_generation_ != 0.f)
			? (statistics_.iterations_ - statistics_.frames_since_last_gen_change) : statistics_.iterations_;
		statistics_.frames_since_last_gen_change = 0;
		tracked_generation_ = statistics_.average_generation;
	}

	float protozoa_count = static_cast<float>(all_protozoa_.size());
	if (protozoa_count == 0)
		return;

	statistics_.average_cells_per_protozoa = 0.f;
	statistics_.average_offspring_count = 0.f;
	statistics_.average_mutation_rate = 0.f;
	statistics_.average_mutation_range = 0.f;

	// Calculating averages for cells per protozoa, offspring count, mutation rate, and mutation range across all protozoa
	int cell_count = 0;
	for (Protozoa* protozoa : all_protozoa_)
	{
		for (Cell& cell : protozoa->get_cells())
		{
			statistics_.average_mutation_rate += cell.mutation_rate;
			statistics_.average_mutation_range += cell.mutation_range;

			cell_count++;
		}
		statistics_.average_cells_per_protozoa += protozoa->get_cells().size();
		statistics_.average_offspring_count += protozoa->offspring_count;
	}

	statistics_.average_cells_per_protozoa /= protozoa_count;
	statistics_.average_offspring_count /= protozoa_count;
	statistics_.average_mutation_rate /= cell_count;
	statistics_.average_mutation_range /= cell_count;

	// Updating death and birth rates per hundred frames
	if (statistics_.iterations_ % survival_rate_window_size_ == 0)
	{
		statistics_.deaths_per_hundered_frames = static_cast<float>(deaths_this_window_);
		statistics_.births_per_hundered_frames = static_cast<float>(births_this_window_);

		// Reset window
		deaths_this_window_ = 0;
		births_this_window_ = 0;
	}

	// Peak population
	const int p_count = static_cast<int>(all_protozoa_.size());
	statistics_.peak_protozoa_ever = std::max(p_count, statistics_.peak_protozoa_ever);

	// Per-organism aggregates
	float total_energy = 0.f;
	float total_springs = 0.f;
	float sum_amp = 0.f, sum_sq = 0.f;
	int   cell_div_count = 0;

	for (Protozoa* p : all_protozoa_)
	{
		total_energy += p->get_energy();
		total_springs += static_cast<float>(p->get_springs().size());
		most_offspring_ever_ = std::max(p->offspring_count, most_offspring_ever_);
		for (const Cell& c : p->get_cells())
		{
			sum_amp += c.amplitude;
			sum_sq += c.amplitude * c.amplitude;
			++cell_div_count;
			statistics_.highest_generation_ever = std::max(c.generation, statistics_.highest_generation_ever);
		}
	}

	if (protozoa_count > 0)
	{
		statistics_.average_energy = total_energy / protozoa_count;
		statistics_.average_spring_count = total_springs / protozoa_count;
		statistics_.energy_efficiency = statistics_.average_energy / 300.f; // ratio vs starting energy
	}
	if (cell_div_count > 0)
	{
		const float mean_amp = sum_amp / cell_div_count;
		statistics_.genetic_diversity = (sum_sq / cell_div_count) - (mean_amp * mean_amp);
	}
}

