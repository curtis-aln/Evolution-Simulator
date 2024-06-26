#pragma once

#include <SFML/Graphics.hpp>
#include "../Utils/utility.h"
#include "../Utils/random.h"


class Cell : CellSettings
{
	Cell* m_parent_ = nullptr;
	std::vector<int> children_ids_{};

	sf::Vector2f m_position_{};
	sf::Vector2f m_velocity_{};

	float m_radius_ = cell_radius;
	float m_friction = 0.99f;

	


public:
	sf::Color cell_color_inner = cell_fill_colors[Random::rand_range(0, static_cast<int>(cell_fill_colors.size()) - 1)];
	sf::Color cell_color_outer = cell_outline_colors[Random::rand_range(0, static_cast<int>(cell_outline_colors.size()) - 1)];

	int rel_id;

	// the player can press and hold on a cell, and will have the ability to drag it around
	bool selected = false;

	// constructor
	Cell(Cell* parent = nullptr, const int rel_id = 0) : m_parent_(parent), rel_id(rel_id)
	{

	}

	void update(std::vector<Cell>& family_cells)
	{
		for (Cell& cell : family_cells)
		{
			if (cell.rel_id != rel_id)
			{
				const sf::Vector2f rel = m_position_ - cell.m_position_;
				m_velocity_ += (rel / length(rel)) * Random::rand_range(-0.02f, 0.05f);
			}
		}

		m_position_ += m_velocity_;
		m_velocity_ *= m_friction;
	}

	void bound(const Circle& bounds)
	{
		if (!bounds.contains(m_position_))
		{
			const sf::Vector2f delta = bounds.center - m_position_;
			m_velocity_ += delta * 0.01f;
		}
	}

	void set_position(const sf::Vector2f new_position)
	{
		m_position_ = new_position;
	}

	void set_velocity(const sf::Vector2f new_velocity)
	{
		m_position_ = new_velocity;
	}


	[[nodiscard]] sf::Vector2f get_position() const
	{
		return m_position_;
	}
	[[nodiscard]] sf::Vector2f get_velocity() const
	{
		return m_velocity_;
	}
	[[nodiscard]] float get_radius() const
	{
		return m_radius_;
	}

	std::vector<int>& get_children_ids() { return children_ids_; };

	// relationship management
	void set_parent(Cell* parent) { m_parent_ = parent; }
	bool is_parent(const Cell* query_cell) const { return query_cell == m_parent_; }

	[[nodiscard]] bool is_child(const int query_cell_id) const
	{
		//todo replce with std ranges
		for (const int id : children_ids_)
		{
			if (id == query_cell_id)
				return true;
		}
		return false;
	}
	void add_child(const int child_id)
	{
		children_ids_.push_back(child_id);
	}


	void accelerate(const sf::Vector2f acceleration)
	{
		m_velocity_ += acceleration;
	}
};