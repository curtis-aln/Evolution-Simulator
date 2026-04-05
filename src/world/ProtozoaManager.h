#pragma once
#include "../Protozoa/Protozoa.h"

// A Class which handles all protozoa related stuff in the world. updating, collisions, reproduction, etc.
class ProtozoaManager : protected WorldSettings
{
protected:
	o_vector<Protozoa> all_protozoa_;

	
public:
	// The user can click on a protozoa to select it for debugging purposes. we store a pointer to it here.
	Protozoa* selected_protozoa_ = nullptr;

	float average_lifetime_ = 0.f; // the average lifetime of the 500 most recent protozoa deaths
	float deaths_per_hundered_frames_ = 0.f; // the amount of protozoa that have died per hundred frames, used to track the death rate of the population
	float births_per_hundered_frames_ = 0.f; // the amount of protozoa that have been born per hundred frames, used to track the birth rate of the population

	std::vector<float> recent_lifetimes_ = {}; // a vector storing the lifetimes of the 500 most recent protozoa deaths, used to calculate average_lifetime_

	static constexpr size_t max_lifetime_samples_ = 500;

	int deaths_this_window_ = 0;
	int births_this_window_ = 0;
	static constexpr int survival_rate_window_size_ = 100;



	// every frame this is filled with the collision resolutions calculated in the collision detection phase, and then applied to the cells in the update phase. 
	// this is done to avoid modifying cell velocities during the collision detection phase which can cause errors in subsequent collision checks within the same frame.
	std::vector<sf::Vector2f> collision_resolutions{};



	ProtozoaManager() : all_protozoa_(max_protozoa)
	{

	}


	Protozoa* get_selected_protozoa() const { return selected_protozoa_; }

	int get_protozoa_count() const
	{
		return all_protozoa_.size();
	}

	float get_average_generation() const
	{
		if (all_protozoa_.size() == 0) 
			return 0.f;

		float sum = 0.f;
		int count = 0;
		for (Protozoa* protozoa : all_protozoa_)
		{
			for (Cell& cell : protozoa->get_cells())
			{
				sum += cell.generation;
				count++;
			}
		}
		return sum / count;
	}


	void deselect_protozoa()
	{
		if (selected_protozoa_ != nullptr)
		{
			selected_protozoa_ = nullptr;
		}
	}

	inline static constexpr int max_evolutionary_iterations = 5;
	inline static constexpr int desired_cell_count = 4;



protected:
	inline void generate_protozoa(Protozoa& protozoa, Circle& world_bounds, bool emplace_back = true)
	{
		if (protozoa.get_cells().size() > 0)
		{
			std::cout << "[WARNING]: Protozoa already has cells, generation process may produce unexpected results.\n";
		}

		// we dont need to create protozoas for the "first time" during extinction or reset events
		protozoa.hard_reset();
		protozoa.m_cells_.emplace_back(0, world_bounds.rand_pos());
		


		for (int i = 0; i < max_evolutionary_iterations; ++i)
		{
			protozoa.mutate(true, 0.2f, 0.03f);

			// as soon as we reach the criteria passing the desired cell count, the chance of breaking out the loop increases 
			// gradually until max evolutionary iterations
			const size_t cell_count = protozoa.get_cells().size();
			if (cell_count < desired_cell_count)
				continue;
			
			float break_chance = float(cell_count - desired_cell_count) / float(max_evolutionary_iterations - desired_cell_count);
			if (Random::rand01_float() < break_chance)
				break;
		}

		protozoa.bound_cells();
	}

	void update_all_protozoa(FoodManager& food_manager_, bool debug_mode, float min_speed, bool track_statistics)
	{
		int idx = 0;
		// resolving collisions
		for (Protozoa* protozoa : all_protozoa_)
		{
			// applying the collision resolution
			for (Cell& cell : protozoa->m_cells_)
			{
				cell.position_ += collision_resolutions[idx++];
			}
		}
		
		std::vector<int> reproduce_indexes{};
		reproduce_indexes.reserve(max_protozoa);

		for (Protozoa* protozoa : all_protozoa_)
		{
			protozoa->update(food_manager_, debug_mode, min_speed);

			if (protozoa->dead)
			{
				if (track_statistics)
					register_death_stat(protozoa->frames_alive);
				all_protozoa_.remove(protozoa);
				continue; // causes indexing issues if the orgasanism is removed before we check for reproduction
			}

			if (protozoa->reproduce)
			{
				reproduce_indexes.push_back(protozoa->id);
			}
		}

		for (int idx : reproduce_indexes)
		{
			create_offspring(all_protozoa_.at(idx));
			if (track_statistics)
				register_birth_stat();
		}
	}

	Protozoa* get_unallocated_protozoa()
	{
		// This function will either return an unallocated protozoa from the pool, or if none are available, 
		// it will return a random existing protozoa to be overwritten.
		Protozoa* offspring = all_protozoa_.add();

		if (offspring == nullptr)
		{
			const size_t idx = Random::rand_range(unsigned(0), all_protozoa_.size() - 1);
			offspring = all_protozoa_.at(idx);
		}
		return offspring;
	}

	void create_offspring(Protozoa* parent)
	{
		Protozoa* offspring = get_unallocated_protozoa();
		parent->reproduce = false;

		// first we assign the genetic aspects of the offspring to match that of the parents, then reconstruct it
		offspring->soft_reset();
		offspring->set_protozoa_attributes(parent);

		// incrementing the generation in all of the cells and springs
		offspring->update_generation();

		offspring->mutate();
		offspring->birth_location = parent->get_center();

		// we offset the offspring's position slightly from the parent as if it spawns directly in its parent
		// it can cause a sudden push on eachovers cells which could result in spring breaking and cell death
		float parent_bounds_x = parent->get_bounds().size.x;
		float parent_bounds_y = parent->get_bounds().size.y;
		float disp_x = Random::rand_range(-parent_bounds_x, parent_bounds_x);
		float disp_y = Random::rand_range(-parent_bounds_y, parent_bounds_y);
		offspring->move_center_location_to(parent->get_center() + sf::Vector2f{ disp_x, disp_y });
	}


	void check_for_extinction_event(Circle& world_bounds)
	{
		if (all_protozoa_.size() > 0)
			return;

		std::cout << "Extinction event occurred, respawning initial protozoa...\n";

		for (int i = 0; i < initial_protozoa; ++i)
		{
			Protozoa* protozoa = all_protozoa_.add();
			generate_protozoa(*protozoa, world_bounds, false);
		}

		
		
	}

	void register_death_stat(float lifetime)
	{
		// Track lifetime
		recent_lifetimes_.push_back(lifetime);
		if (recent_lifetimes_.size() > max_lifetime_samples_)
			recent_lifetimes_.erase(recent_lifetimes_.begin());

		// Update average lifetime
		float sum = 0.f;
		for (float l : recent_lifetimes_)
			sum += l;

		average_lifetime_ = recent_lifetimes_.empty() ? 0.f : sum / recent_lifetimes_.size();

		// Track death rate
		deaths_this_window_++;
	}

	void register_birth_stat()
	{
		births_this_window_++;
	}
};