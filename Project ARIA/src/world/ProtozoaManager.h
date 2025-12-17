#pragma once
#include "../Protozoa/Protozoa.h"


// A Class which handles all protozoa related stuff in the world. updating, collisions, reproduction, etc.
class ProtozoaManager : protected WorldSettings
{
protected:
	Protozoa::Protozoa_Vector all_protozoa_{};

	// The user can click on a protozoa to select it for debugging purposes. we store a pointer to it here.
	Protozoa* selected_protozoa_ = nullptr;

	// All cells are stored inside their respective protozoa, but we also maintain a global 
	// vector of cell pointers for easy access during collision detection and updates.
	std::vector<Cell*> global_cell_vector_;

	// statistics - To be displayed on the screen
	float average_cells_per_protozoa_; // TODO
	float average_offspring_count_;
	float average_mutation_rate_;
	float average_mutation_range_;


	
public:
	ProtozoaManager()
	{
		
	}

	Protozoa* get_selected_protozoa() const { return selected_protozoa_; }

	int get_protozoa_count() const
	{
		return all_protozoa_.size();
	}

	float get_average_generation() const
	{
		float sum = 0.f;
		for (Protozoa* protozoa : all_protozoa_)
		{
			sum += protozoa->generation;
		}
		return sum / all_protozoa_.size();
	}


	void deselect_protozoa()
	{
		if (selected_protozoa_ != nullptr)
		{
			selected_protozoa_->deselect_cell();
			selected_protozoa_ = nullptr;
		}
	}


protected:
	void update_all_protozoa(FoodManager& food_manager_, bool debug_mode)
	{
		std::vector<int> reproduce_indexes{};
		reproduce_indexes.reserve(max_protozoa);
		for (Protozoa* protozoa : all_protozoa_)
		{
			protozoa->update(food_manager_, debug_mode);

			if (protozoa->reproduce)
			{
				reproduce_indexes.push_back(protozoa->id);
			}

			if (protozoa->dead)
			{
				all_protozoa_.remove(protozoa);
			}
		}

		for (int idx : reproduce_indexes)
		{
			create_offspring(all_protozoa_.at(idx));
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
		offspring->reset_protozoa();
		offspring->set_protozoa_attributes(parent);
		offspring->generation += 1;
		offspring->mutate();
	}

	void update_global_cell_vector()
	{
		global_cell_vector_.clear();

		for (Protozoa* protozoa : all_protozoa_)
		{
			for (Cell& cell : protozoa->get_cells())
			{
				global_cell_vector_.push_back(&cell);
			}
		}
	}


	void check_for_extinction_event()
	{
		// if there are no protozoa left, we need to respawn some and create random genes for them
		if (all_protozoa_.size() == 0)
		{
			for (int i = 0; i < initial_protozoa; ++i)
			{
				Protozoa* protozoa = all_protozoa_.add();
				protozoa->generate_random();
			}
		}
	}
};