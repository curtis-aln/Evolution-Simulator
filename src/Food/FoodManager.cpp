#include "food_manager.h"

FoodManager::FoodManager(sf::RenderWindow* window, Circle* world_circular_bounds)
	: world_bounds_(world_circular_bounds), food_renderer(window), window_(window)
{
	init_food();

	food_positions.resize(max_food, {});
	food_colors.resize(max_food, {});
	food_radii.resize(max_food, food_radius);

	food_renderer.init(food_radius, max_food);
}

void FoodManager::update()
{
	spawn_food_improved(); //spawn_food();
	update_food();
	update_hash_grid();
}

void FoodManager::render()
{
	int idx = 0;
	for (Food* food : food_vector)
	{
		food_positions[idx] = food->position;

		sf::Color c = food->color;

		float age = static_cast<float>(food->age);
		c.a = std::min(age / kFoodVisibilityRampFrames, 1.f) * kFoodMaxAlpha;

		food_colors[idx] = c;

		idx++;
	}

	food_renderer.update_colors(food_colors, idx);
	food_renderer.render(food_positions, food_radii, idx);
}

// world interacting with the food
void FoodManager::remove_food(const int food_id)
{
	Food* food = food_vector.at(food_id);
	food->position = { 0, 0 };
	food->age = 0;
	food->time_since_last_reproduced = 0;
	food->nutrients = initial_nutrients;
	food_vector.remove(food_id);
}

Food* FoodManager::at(const int idx)
{
	return food_vector.at(idx);
}

void FoodManager::draw_food_grid(sf::Vector2f mouse_pos)
{
	food_grid_renderer.render_grid(*window_, mouse_pos, 1600.f);
}

int FoodManager::get_size() const
{
	return food_vector.size();
}
