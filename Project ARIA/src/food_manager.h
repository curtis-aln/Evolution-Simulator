#pragma once

#include <SFML/Graphics.hpp>
#include "Utils/Graphics/buffer_renderer.h" // to render all the food
#include "Utils/o_vector.hpp" // to allow food to be created and destroyed
#include "Utils/Graphics/Circle.h" // world boundary
#include "Utils/random.h"
#include "settings.h"
#include "Utils/Graphics/spatial_hash_grid.h"

struct Food
{
	int id = 0;

	sf::Vector2f position{};
	sf::Vector2f velocity{};

	bool active = true;
};

class FoodManager : FoodSettings
{
	sf::RenderWindow* window_ = nullptr;
	Circle* world_bounds_ = nullptr;

	CircleBuffer food_renderer{ window_ };
	o_vector<Food, max_food> food_vector{};

	std::vector<sf::Vector2f> food_positions;

public:
	const float bounds_radius = world_bounds_->radius;
	const sf::FloatRect world_bounds = { 0, 0, bounds_radius * 2, bounds_radius * 2 };
	SpatialHashGrid<cells_x, cells_y, cell_capacity> spatial_hash_grid{ world_bounds };
	const uint8_t max_nearby_capacity = spatial_hash_grid.max_nearby_capacity;

	int frames = 0;

public:
	FoodManager(sf::RenderWindow* window, Circle* world_bounds) : window_(window), world_bounds_(world_bounds)
	{
		init_renderer();
		init_food();

		food_positions.reserve(max_food);
	}

	int get_size()
	{
		return food_vector.size();
	}


	void update()
	{
		int size = food_vector.size();
		if (size < initial_food && frames++ % 2 == 0)
		{
			for (int i = 0; i < 9; ++i)
			{
				Food* food = food_vector.add();
				if (food == nullptr)
					break;
				food->position = Random::rand_pos_in_circle(world_bounds_->center, world_bounds_->radius);
			}
		}

		spatial_hash_grid.clear();
		for (Food* food : food_vector)
		{
			spatial_hash_grid.addAtom(food->position, food->id);
		}
	}

	void render()
	{
		food_positions.clear();

		for (Food* food : food_vector)
		{
			food_positions.push_back(food->position);
		}

		food_renderer.render(food_positions);
	}

	// world interacting with the food
	void remove_food(int food_id)
	{
		Food* food = food_vector.at(food_id);
		food->position = { 0, 0 };
		food_vector.remove(food_id);
	}

	Food* at(int idx)
	{
		return food_vector.at(idx);
	}

private:
	void init_renderer()
	{
		// 
		std::vector<sf::Color> colors;
		colors.resize(max_food);
		for (int i = 0; i < max_food; ++i)
		{
			sf::Vector3i rgb_min = { 0, 160, 0 };
			sf::Vector3i rgb_max = { 80, 255, 100 };
			colors[i] = Random::rand_color(rgb_min, rgb_max);
		}

		food_renderer.init_texture(colors, food_radius, max_food);
	}

	void init_food()
	{
		for (int i = 0; i < max_food; ++i)
		{
			Food food;
			food.id = i;
			food.position = Random::rand_pos_in_circle(world_bounds_->center, world_bounds_->radius);
			food.velocity = Random::rand_vector(-10.f, 10.f);
			food_vector.emplace(food);
		}

		for (int i = 0; i < max_food - initial_food; ++i)
		{ 
			food_vector.remove(i);
		}
	}
};