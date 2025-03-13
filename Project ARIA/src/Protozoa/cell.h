#pragma once

#include <SFML/Graphics.hpp>

#include "genetics.h"
#include "../Utils/utility.h"
#include "../Utils/random.h"

// Each organism consists of cells which work together via springs
// Each cell has their own radius and friction coefficient, as well as cosmetic factors such as color

inline static constexpr float border_repulsion_magnitude = 0.001f; // how strong it is repelled from the border
inline static const float max_speed = 30;

class Cell : public CellGene
{
public:
	sf::Vector2f position_{};
	sf::Vector2f velocity_{};

	Cell(const CellGene& info) : CellGene(info)
	{}


	void update()
	{
		// updating velocity and position vectors
		clamp_velocity();
		position_ += velocity_;
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