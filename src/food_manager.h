#pragma once

#include <SFML/Graphics.hpp>
#include "Utils/Graphics/CircleBatchRenderer.h" // to render all the food
#include "Utils/o_vector.hpp" // to allow food to be created and destroyed
#include "Utils/Graphics/Circle.h" // world boundary
#include "Utils/random.h"
#include "settings.h"
#include "Utils/Graphics/SFML_Grid.h"
#include "Utils/Graphics/spatial_grid/simple_spatial_grid.h"


struct Food
{
	int id = 0;
	int age = 0;
	int time_since_last_reproduced = 0;

	sf::Vector2f position{};
	sf::Vector2f velocity{};
	sf::Color color{};

	bool active = true;
};



class FoodManager : FoodSettings
{
	Circle* world_bounds_ = nullptr;

	CircleBatchRenderer food_renderer;
	o_vector<Food> food_vector{max_food};

	std::vector<sf::Vector2f> food_positions;
	std::vector<sf::Color> food_colors;

public:
	const float bounds_radius = world_bounds_->radius;
	const sf::FloatRect world_rect_bounds = { {0, 0}, {bounds_radius * 2, bounds_radius * 2} };

	SimpleSpatialGrid spatial_hash_grid{ cells_x, cells_y, cell_max_capacity, bounds_radius * 2, bounds_radius * 2 };
	SFML_Grid food_grid_renderer; // renders the food spatial hash grid

	int frames = 0;

public:
	FoodManager(sf::RenderWindow* window, Circle* world_circular_bounds) 
	: 
		world_bounds_(world_circular_bounds), food_renderer(window),
		food_grid_renderer(*window, world_rect_bounds, cells_x, cells_y)
	{
		init_food();

		food_positions.resize(max_food, {});
		food_colors.resize(max_food, {});

	}

	int get_size() const
	{
		return food_vector.size();
	}


	void update()
	{
		spawn_food_improved(); //spawn_food();
		vibrate_food();
		update_hash_grid();
	}

	void render()
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

		food_renderer.init_texture(food_colors, food_radius, idx);
		food_renderer.render(food_positions, idx);
	}

	// world interacting with the food
	void remove_food(const int food_id)
	{
		Food* food = food_vector.at(food_id);
		food->position = { 0, 0 };
		food->age = 0;
		food->time_since_last_reproduced = 0;
		food_vector.remove(food_id);
	}

	Food* at(const int idx)
	{
		return food_vector.at(idx);
	}

	void draw_food_grid()
	{
		food_grid_renderer.draw();
	}

private:
	void spawn_food()
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

	void spawn_food_improved()
	{
		int size = food_vector.size();
		if (food_vector.size() > max_food)
			return;

		for (Food* food : food_vector)
		{
			food->time_since_last_reproduced++;

			if (food->time_since_last_reproduced < reproductive_cooldown)
				continue;

			float spawn_chance = 1.f - static_cast<float>(size) / static_cast<float>(max_food);
			spawn_chance = std::clamp(spawn_chance * spawn_proportionality_constant, 0.f, 1.f);

			if (Random::rand01_float() > spawn_chance)
				continue;

			Food* new_food = food_vector.add();
			if (new_food == nullptr)
				break;
			// spawning the food next to another existing food 
			sf::Vector2f other_food_pos = food->position;
			
			new_food->position = Random::rand_pos_in_circle(other_food_pos, food_radius * food_spawn_distance);
			new_food->age = 0.f;
			new_food->color = Random::rand_color(food_darkest_color, food_lightest_color);

			food->time_since_last_reproduced = 0;
		}
	}

	void vibrate_food()
	{
		for (Food* food : food_vector)
		{
			food->age++;

			food->velocity = Random::rand_vector(-vibration_strength, vibration_strength);
			food->velocity *= friction;
			food->position += food->velocity;

			// keep the food within the world bounds
			const float dist_sq = (food->position - world_bounds_->center).lengthSquared();

			const float max_dist = bounds_radius - food_radius;
			if (dist_sq > max_dist * max_dist)
			{
				const sf::Vector2f to_center = world_bounds_->center - food->position;
				const sf::Vector2f normal = to_center.normalized();
				food->position = world_bounds_->center + normal * max_dist;
			}
		}
	}

	void update_hash_grid()
	{
		if (frames % update_freq != 0)
			return;

		spatial_hash_grid.clear();
		for (Food* food : food_vector)
		{
			spatial_hash_grid.add_object(food->position.x, food->position.y, food->id);
		}
	}

	void init_food()
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
};