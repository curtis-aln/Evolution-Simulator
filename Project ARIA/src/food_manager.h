#pragma once

#include <SFML/Graphics.hpp>
#include "Utils/Graphics/buffer_renderer.h" // to render all the food
#include "Utils/o_vector.hpp" // to allow food to be created and destroyed
#include "Utils/Graphics/Circle.h" // world boundary
#include "Utils/random.h"
#include "settings.h"

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

public:
	FoodManager(sf::RenderWindow* window, Circle* world_bounds) : window_(window), world_bounds_(world_bounds)
	{
		init_renderer();
		init_food();
	}


	void update()
	{

	}

	void render()
	{
		std::vector<sf::Vector2f> food_positions;
		food_positions.reserve(food_vector.size());

		for (Food* food : food_vector)
		{
			food_positions.push_back(food->position);
		}

		food_renderer.render(food_positions);
	}

	// world interacting with the food
	void remove_food(int food_id)
	{
		food_vector.remove(food_id);
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