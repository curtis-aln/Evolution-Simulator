#include "food_manager.h"

void FoodManager::spawn_food()
{
	int size = food_vector.size();
	if (food_vector.size() > max_food)
		return;

	if (frames++ % food_spawn_interval != 0)
		return;

	for (int i = 0; i < food_spawn_amount; ++i)
	{
		Food* food = food_vector.add();

		if (food == nullptr)
			break;

		// spawning the food next to another existing food 
		sf::Vector2f other_food_pos =
			food->position = Random::rand_pos_in_circle(world_bounds_->center, world_bounds_->radius);
		food->age = 0.f;
		food->color = Random::rand_color(food_darkest_color, food_lightest_color);
	}

}

bool FoodManager::food_container_full()
{
	return food_vector.size() >= max_food;
}

bool FoodManager::can_food_reproduce(Food* food)
{
	if (food->time_since_last_reproduced < reproductive_cooldown)
		return false;
	if (food->age < reproductive_threshold)
		return false;
	return true;
}

float FoodManager::calculate_spawn_chance()
{
	float spawn_chance = 1.f - static_cast<float>(food_vector.size()) / static_cast<float>(max_food);
	return std::clamp(spawn_chance * spawn_proportionality_constant, 0.f, 1.f);
}

void FoodManager::spawn_food_improved()
{
	if (food_container_full())
		return;

	for (Food* food : food_vector)
	{
		if (!can_food_reproduce(food))
			continue;

		if (Random::rand01_float() > calculate_spawn_chance())
			continue;

		bool reached_max_food = reproduce_food(food);

		if (reached_max_food)
			break;
	}
}

bool FoodManager::reproduce_food(Food* food)
{
	Food* new_food = food_vector.add();
	if (new_food == nullptr)
		return true; // max food has been reached

	// spawning the food next to another existing food 
	sf::Vector2f other_food_pos = food->position;

	new_food->position = Random::rand_pos_in_circle(other_food_pos, food_radius * food_spawn_distance);
	new_food->age = 0.f;
	new_food->color = Random::rand_color(food_darkest_color, food_lightest_color);

	food->time_since_last_reproduced = 0;
	return false;
}