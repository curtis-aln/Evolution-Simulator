#include "../Protozoa.h"
#include "../../food_manager.h"


void Protozoa::handle_food(FoodManager& food_manager)
{
	const sf::Vector2f center = get_center();
	c_Vec<food_manager.spatial_hash_grid.max_nearby_capacity>& nearby =
		food_manager.spatial_hash_grid.find(center);

	food_positions_nearby.clear();

	for (int i = 0; i < nearby.size; ++i)
	{
		Food* food_particle = food_manager.at(nearby.at(i));
		food_positions_nearby.push_back(food_particle->position);

		if (m_personal_bounds_.contains(food_particle->position)) // todo better collision handling
		{
			food_manager.remove_food(food_particle->id);
			reproduce = true;
		}
	}

}

inline static constexpr float cell_mutation_rate = .3f;
inline static constexpr float cell_mutation_range = 3.f;
inline static constexpr float spring_mutation_rate = .3f;
inline static constexpr float spring_mutation_range = 3.f;

void Protozoa::mutate()
{
	// mutating the cells in this organism
	for (Cell& cell : m_cells_)
	{
		if (Random::rand01_float() < cell_mutation_rate)
		{
			cell.vibration += Random::rand11_float() * cell_mutation_range;
		}
	}

	// mutating the springs in this organism
	for (Spring& spring : m_springs_)
	{
		if (Random::rand01_float() < spring_mutation_rate)
		{
			spring.vibration += Random::rand11_float() * spring_mutation_range;
		}
	}
}