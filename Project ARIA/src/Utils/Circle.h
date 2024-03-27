#pragma once
#include <SFML/System/Vector2.hpp>
#include "Random.h"

struct Circle
{
	sf::Vector2f center{};
	float radius{};
	

	Circle(const sf::Vector2f circle_center = {}, const float circle_radius = 0.f)
		: center(circle_center), radius(circle_radius)
	{
		
	}

	[[nodiscard]] bool contains(const sf::Vector2f point) const
	{
		// if the distance from the center squared is greater than the radius squared it is out of bounds
		const sf::Vector2f delta = point - center;
		const float dist_sq = delta.x * delta.x + delta.y * delta.y;
		const float rad_sq = radius * radius;

		return dist_sq < rad_sq;
	}

	sf::Vector2f rand_pos_in_circle() const
	{
		const sf::Rect bounds = { center.x - radius, center.y - radius, radius * 2, radius * 2 };
		for (int i = 0; i < 20; ++i)
		{
			if (const sf::Vector2f pos = Random::rand_pos_in_rect(bounds); contains(pos))
			{
				return pos;
			}
		}

		std::cout << "ERROR Circle.h rand_pos_in_circle() failed\n";
		return { 0, 0 };
	}
};
