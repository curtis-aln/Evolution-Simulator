#pragma once

#include <SFML/Graphics.hpp>

#include "genetics.h"
#include "../Utils/utility.h"
#include "../Utils/random.h"


class Cell : CellSettings
{
	float m_radius_ = 0.f;
	float m_friction = 0.99f;



public:
	sf::Color m_color_inner_;
	sf::Color m_color_outer_;

	sf::Vector2f position{};
	sf::Vector2f velocity{};

	int rel_id;

	// the player can press and hold on a cell, and will have the ability to drag it around
	bool selected = false;


	// constructor
	Cell(const int rel_id = 0, const float radius = 0, sf::Color color_inner = {}, sf::Color color_outer = {})
	: m_radius_(radius), m_color_inner_(color_inner), m_color_outer_(color_outer), rel_id(rel_id)
	{}

	Cell(const CellGenetics& info = CellGenetics())
		: m_radius_(info.radius), m_color_inner_(info.colors.first), m_color_outer_(info.colors.second), rel_id(info.id)
	{}


	void update(std::vector<Cell>& family_cells)
	{
		for (Cell& cell : family_cells)
		{
			if (cell.rel_id != rel_id)
			{
				const sf::Vector2f rel = position - cell.position;
				velocity += (rel / length(rel)) * Random::rand_range(-0.02f, 0.05f);
			}
		}

		position += velocity;
		velocity *= m_friction;
	}


	void bound(const Circle& bounds)
	{
		if (!bounds.contains(position))
		{
			const sf::Vector2f delta = bounds.center - position;
			velocity += delta * 0.01f;
		}
	}


	[[nodiscard]] float get_radius() const
	{
		return m_radius_;
	}


	void accelerate(const sf::Vector2f acceleration)
	{
		velocity += acceleration;
	}
};