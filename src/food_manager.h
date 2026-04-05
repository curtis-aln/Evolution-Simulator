#pragma once

#include <SFML/Graphics.hpp>
#include "Utils/Graphics/CircleBatchRenderer.h" // to render all the food
#include "Utils/o_vector.hpp" // to allow food to be created and destroyed
#include "Utils/Graphics/Circle.h" // world boundary
#include "Utils/random.h"
#include "settings.h"
#include "Utils/Graphics/simple_spatial_grid.h"
#include "Utils/Graphics/SFML_Grid.h"


struct Food
{
	int id = 0;
	int age = 0;

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

	SimpleSpatialGrid<cells_x, cells_y, cell_capacity> spatial_hash_grid{ world_rect_bounds };
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
		spawn_food();
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

			food->position = Random::rand_pos_in_circle(world_bounds_->center, world_bounds_->radius);
			food->age = 0.f;
			food->color = Random::rand_color(food_darkest_color, food_lightest_color);
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

			if (dist_sq > bounds_radius * bounds_radius)
			{
				const sf::Vector2f to_center = world_bounds_->center - food->position;
				const sf::Vector2f normal = to_center.normalized();
				food->position = world_bounds_->center + normal * bounds_radius;
			}
		}
	}

	void update_hash_grid()
	{
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