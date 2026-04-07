#include "food_manager.h"

void FoodManager::vibrate_food(Food* food)
{
	food->velocity = Random::rand_vector(-vibration_strength, vibration_strength);
}

void FoodManager::update_food()
{
	for (Food* food : food_vector)
	{
		food->time_since_last_reproduced++;
		food->age++;
		vibrate_food(food);

		food->velocity *= friction;
		food->position += food->velocity;

		bound_food_to_world(food);

		update_food_nutrients(food);
		check_food_death(food);
	}
}

void FoodManager::bound_food_to_world(Food* food)
{
	const float dist_sq = (food->position - world_bounds_->center).lengthSquared();

	const float max_dist = bounds_radius - food_radius;

	if (dist_sq < max_dist * max_dist)
		return;

	const sf::Vector2f to_center = world_bounds_->center - food->position;
	const sf::Vector2f normal = to_center.normalized();
	food->position = world_bounds_->center + normal * max_dist;
}

void FoodManager::check_food_death(Food* food)
{
	if (food->age < death_age)
		return;

	if (Random::rand01_float() < death_age_chance)
		remove_food(food->id);
}


void FoodManager::update_food_nutrients(Food* food)
{
	const float increment = (final_nuterients - initial_nutrients) / nutrient_development_time;
	if (food->nutrients > final_nuterients)
		return;

	food->nutrients += increment;
	if (food->nutrients > final_nuterients)
		food->nutrients = final_nuterients;

}

void FoodManager::update_hash_grid()
{
	if (frames % update_freq != 0)
		return;

	spatial_hash_grid.clear();
	for (Food* food : food_vector)
	{
		spatial_hash_grid.add_object(food->position.x, food->position.y, food->id);
	}
}

void FoodManager::init_food()
{
	for (int i = 0; i < max_food; ++i)
	{
		Food food;
		food.id = i;
		food.position = Random::rand_pos_in_circle(world_bounds_->center, world_bounds_->radius);
		food.velocity = Random::rand_vector(-10.f, 10.f);
		food.color = Random::rand_color(food_darkest_color, food_lightest_color);
		food_vector.emplace(food);
	}

	for (int i = 0; i < max_food - initial_food; ++i)
	{
		food_vector.remove(i);
	}
}