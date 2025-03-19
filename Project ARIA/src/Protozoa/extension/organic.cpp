#include "../Protozoa.h"
#include "../../food_manager.h"


void Protozoa::handle_food(FoodManager& food_manager)
{
	const sf::Vector2f center = get_center();
	c_Vec<food_manager.spatial_hash_grid.max_nearby_capacity>& nearby =
		food_manager.spatial_hash_grid.find(center);

	for (int i = 0; i < nearby.size; ++i)
	{
		Food* food_particle = food_manager.at(nearby.at(i));

		if (m_personal_bounds_.contains(food_particle->position)) // todo better collision handling
		{
			food_manager.remove_food(food_particle->id);
			reproduce = true;
		}
	}

}

inline static constexpr float cell_mutation_rate = 0.01f;
inline static constexpr float cell_mutation_range = 0.01f;
inline static constexpr float spring_mutation_rate = 0.01f;
inline static constexpr float spring_mutation_range = 0.01f;

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