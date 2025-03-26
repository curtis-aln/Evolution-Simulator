#pragma once

#include <SFML/Graphics.hpp>

#include "../Utils/utility.h"
#include "../Utils/random.h"

// Each organism consists of cells which work together via springs
// Each cell has their own radius and friction coefficient, as well as cosmetic factors such as color

inline static constexpr float border_repulsion_magnitude = 0.001f; // how strong it is repelled from the border
inline static const float max_speed = 30;


struct Cell : public CellGeneSettings
{
	int id{}; // Unique identifier relative to the protozoa
	int protozoa_id{}; // Unique identifier pointing to its protozoa

	float radius = CellSettings::cell_radius;
		
	float offset = Random::rand_range(offset_range);
	float frequency = Random::rand_range(frequency_range);

	sf::Vector2f position_{};
	sf::Vector2f velocity_{};


	Cell(const int _id, const int _protozoa_id, const sf::Vector2f position)
		: id(_id), protozoa_id(_protozoa_id), position_(position)
	{

	}

	void update(int internal_clock)
	{
		// updating velocity and position vectors
		clamp_velocity();
		position_ += velocity_;

		// we first need to scale down the internal clock value, it increments by 1 every frame which is way too large for the sin input
		// as it is expecting a radian input rather than a degree-like input
		const float scaled_clock = fmod(internal_clock, 360.f) * pi / 180.f;

		// getting a value between 0 and 1 for maximum and minimum friction
		const float phase = (sin(frequency * scaled_clock + offset) + 1.f) / 2.f;

		// changing the range of friction to something better
		const float friction = minFriction + (phase * (maxFriction - minFriction));
		
		// updating the velocity with the new friction
		velocity_ *= friction;
	}


	void bound(const Circle& bounds)
	{
		const sf::Vector2f direction = position_ - bounds.center;
		const float dist_sq = dist_squared(position_, bounds.center);
		const float bounds_radius = bounds.radius - radius - radius * 0.1f; // Ensure the entire circle stays inside
		const float bounds_radius_sq = bounds_radius * bounds_radius;

		if (dist_sq > bounds_radius_sq) // Outside the boundary
		{
			const float dist = std::sqrt(dist_sq);
			sf::Vector2f normal = direction / dist; // Normalize direction

			// Move the circle back inside
			position_ = bounds.center + normal * bounds_radius;

			// Apply velocity adjustment to prevent escaping
			velocity_ -= normal * (border_repulsion_magnitude * (dist - bounds_radius));
		}
	}



	void accelerate(const sf::Vector2f acceleration)
	{
		velocity_ += acceleration;
	}

	void call_mutate(float mutation_rate, float mutation_range)
	{
		if (Random::rand01_float() < mutation_rate)
		{
			offset += Random::rand11_float() * mutation_range;
			frequency += Random::rand11_float() * mutation_range;
		}
	}

private:
	void clamp_velocity()
	{
		const float speed_squared = velocity_.x * velocity_.x + velocity_.y * velocity_.y;

		if (speed_squared > max_speed * max_speed)
		{
			const float speed = sqrt(speed_squared);
			velocity_ *= max_speed / speed;
		}
	}
};