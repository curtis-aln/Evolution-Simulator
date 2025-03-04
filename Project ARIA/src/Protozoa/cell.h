#pragma once

#include <SFML/Graphics.hpp>

#include "genetics.h"
#include "../Utils/utility.h"
#include "../Utils/random.h"

// Each organism consists of cells which work together via springs
// Each cell has their own radius and friction coefficient, as well as cosmetic factors such as color


class Cell : CellSettings
{
	float m_radius_ = 0.f;
	float m_friction = 0.99f; // friction coefficient


public:
	sf::Color color_;
	sf::Color outline_color_;

	sf::Vector2f position_{};
	sf::Vector2f velocity_{};

	int rel_id;

	// the player can press and hold on a cell, and will have the ability to drag it around
	bool selected = false; // todo move outside of this cell class


	// constructors
	Cell(const int rel_id = 0, const float radius = 0, sf::Color color_inner = {}, sf::Color color_outer = {})
	: m_radius_(radius), color_(color_inner), outline_color_(color_outer), rel_id(rel_id)
	{}

	Cell(const CellGenetics& info = CellGenetics())
		: m_radius_(info.radius), color_(info.colors.first), outline_color_(info.colors.second), rel_id(info.id)
	{}


	void update(std::vector<Cell>& family_cells)
	{
		// TODO: this is a makeshift showcase to test the mechanics, no logic is happening here
		static constexpr float R = 0.27f;
		int i = 0;
		for (Cell& cell : family_cells)
		{
			if (cell.rel_id != rel_id) // cells cant interact with themselves
			{
				const sf::Vector2f direction = normalize(position_ - cell.position_);
				//const float magnitude = Random::rand_range(-R, R);
				const float magnitude = R * ((i++ % 2 == 0) * 2 - 1);
				velocity_ += direction * magnitude;
			}
		}

		clamp_velocity();
		position_ += velocity_;
		velocity_ *= m_friction;
	}


	void bound(const Circle& bounds)
	{
		// if the cell is not within the world bounds we accelerate it towards the center
		constexpr float mag = 0.01f;
		if (!bounds.contains(position_))
		{
			const sf::Vector2f delta = bounds.center - position_;
			velocity_ += delta * mag;
		}
	}


	[[nodiscard]] float get_radius() const
	{
		return m_radius_;
	}


	void accelerate(const sf::Vector2f acceleration)
	{
		velocity_ += acceleration;
	}

private:
	void clamp_velocity()
	{
		static const float max_speed = m_radius_ / 2.f; // todo is having it tied to radius a good idea? what if radius changes
		const float speed_squared = velocity_.x * velocity_.x + velocity_.y * velocity_.y;

		if (speed_squared > max_speed * max_speed)
		{
			const float speed = sqrt(speed_squared);
			velocity_ *= max_speed / speed;
		}
	}
};