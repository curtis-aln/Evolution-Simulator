#pragma once
#include <SFML/Graphics.hpp>
#include "../settings.h"
#include "../Utils/random.h"
#include "cell.h"

class Spring : SpringSettings
{
	float m_rest_length_ = rest_length;
	float m_spring_constant_ = spring_constant;
	float m_damping_factor_ = damping_factor;

public:
	int m_cellA_id{};
	int m_cellB_id{};

	sf::Color fill_color;
	sf::Color outline_color;

	// for debugging
	sf::Vector2f direction_A_force{};
	sf::Vector2f direction_B_force{};

public:
	Spring(const int cell_a_id, const int cell_b_id) : m_cellA_id(cell_a_id), m_cellB_id(cell_b_id)
	{
		init_colors();
	}

	void update(Cell& cell_a, Cell& cell_b)
	{
		const sf::Vector2f pos_a = cell_a.get_position();
		const sf::Vector2f pos_b = cell_b.get_position();
		const sf::Vector2f vel_a = cell_a.get_velocity();
		const sf::Vector2f vel_b = cell_b.get_velocity();

		const float dist = length(pos_b - pos_a);

		// Calculating the spring force: Fs = K * (|B - A| - L)
		const float spring_force = m_spring_constant_ * (dist - m_rest_length_);

		// Calculating the damping force
		const sf::Vector2f normalised_dir = ((pos_b - pos_a) / dist);
		const sf::Vector2f vel_difference = (vel_b - vel_a);
		const float damping_force = dot(normalised_dir, vel_difference) * m_damping_factor_;

		// Calculating total force (sum of the two forces)
		const float total_force = spring_force + damping_force;

		// moving each cell
		direction_A_force = total_force * ((pos_b - pos_a) / dist);
		cell_a.accelerate(direction_A_force);

		direction_B_force = total_force * ((pos_a - pos_b) / dist);
		cell_b.accelerate(direction_B_force);
	}


private:
	void init_colors()
	{
		const size_t max_fill = CellSettings::spring_fill_colors.size() - 1;
		const size_t rand_idx_fill = Random::rand_range(static_cast<size_t>(0), max_fill);
		fill_color = CellSettings::spring_fill_colors[rand_idx_fill];

		const size_t max_outline = CellSettings::spring_outline_colors.size() - 1;
		const size_t rand_idx_outline = Random::rand_range(static_cast<size_t>(0), max_outline);
		outline_color = CellSettings::spring_outline_colors[rand_idx_outline];
	}
};
