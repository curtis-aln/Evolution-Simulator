#pragma once

#include <SFML/Graphics.hpp>

#include "genetics.h"
#include "../Utils/utility.h"
#include "../Utils/random.h"

// Each organism consists of cells which work together via springs
// Each cell has their own radius and friction coefficient, as well as cosmetic factors such as color

inline static constexpr float border_repulsion_magnitude = 0.01f; // how strong it is repelled from the border
inline static const float max_speed = 70;

class Cell : public CellGene
{
public:
	sf::Vector2f position_{};
	sf::Vector2f velocity_{};

	Cell(const CellGene& info) : CellGene(info)
	{}


	void update(std::vector<Cell>& family_cells)
	{
		// TODO: this is a makeshift showcase to test the mechanics, no logic is happening here
		static constexpr float R = 0.27f;
		int i = 0;
		for (Cell& cell : family_cells)
		{
			if (cell.id == id) // cells cant interact with themselves
				continue;
			
			// interacting with the other cell
			const sf::Vector2f direction = normalize(position_ - cell.position_);
			const float magnitude = R * ((i++ % 2 == 0) * 2 - 1);
			velocity_ += direction * magnitude;
		}

		// updating velocity and position vectors
		clamp_velocity();
		position_ += velocity_;
		velocity_ *= friction;
	}


	void bound(const Circle& bounds)
	{
		if (!bounds.contains(position_))
		{
			const sf::Vector2f direction = bounds.center - position_;
			velocity_ += direction * border_repulsion_magnitude;
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