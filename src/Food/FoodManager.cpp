#include "food_manager.h"

FoodManager::FoodManager(sf::RenderWindow* window, Circle* world_circular_bounds)
	: world_bounds_(world_circular_bounds), food_renderer(window, food_radius, max_food), window_(window)
{
	init_food();

	food_positions.resize(max_food, {});
	food_colors.resize(max_food, {});
	food_radii.resize(max_food, food_radius);

	food_renderer.set_colors(&food_colors);
	food_renderer.set_positions(&food_positions);
	food_renderer.set_radii(&food_radii);
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
		sf::Color ck = food->color;

		float age = static_cast<float>(food->age);
		c.a = std::min(age / kFoodVisibilityRampFrames, 1.f) * kFoodMaxAlpha;

		food_colors[idx] = c;

		idx++;
	}

	food_renderer.set_size(idx);
	food_renderer.update();
	food_renderer.render();
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

void FoodManager::draw_food_grid(sf::Vector2f mouse_pos) const
{
	food_grid_renderer.render(*window_, mouse_pos, 1600.f);
}

int FoodManager::get_size() const
{
	return food_vector.size();
}


void FoodManager::update_food_grid_renderer()
{
	food_grid_renderer.rebuild();
}