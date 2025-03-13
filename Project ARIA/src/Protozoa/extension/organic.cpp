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