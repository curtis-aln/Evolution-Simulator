#pragma once
#include <SFML/Graphics.hpp>
#include "../settings.h"
#include "../Utils/random.h"
#include "cell.h"

class Spring
{
	float rest_length_ = 40.f;
	float spring_constant_ = 0.05f;
	float damping_factor_ = 0.95f;

public:
	int m_cellA_id{};
	int m_cellB_id{};

	sf::Color fill_color = CellSettings::spring_fill_colors[Random::rand_range(0, int(CellSettings::spring_fill_colors.size()) - 1)];
	sf::Color outline_color = CellSettings::spring_outline_colors[Random::rand_range(0, int(CellSettings::spring_outline_colors.size()) - 1)];

public:
	Spring(const int cellA_id, const int cellB_id) : m_cellA_id(cellA_id), m_cellB_id(cellB_id)
	{

	}

	void update(Cell& cellA, Cell& cellB) const
	{
		const sf::Vector2f posA = cellA.get_position();
		const sf::Vector2f posB = cellB.get_position();
		const sf::Vector2f velA = cellA.get_velocity();
		const sf::Vector2f velB = cellB.get_velocity();

		const float dist = length(posB - posA);

		// Calculating the spring force: Fs = K * (|B - A| - L)
		const float spring_force = spring_constant_ * (dist - rest_length_);

		// Calculating the damping force
		const sf::Vector2f normalised_dir = ((posB - posA) / dist);
		const sf::Vector2f vel_difference = (velB - velA);
		const float damping_force = dot(normalised_dir, vel_difference) * damping_factor_;

		// Calculating total force (sum of the two forces)
		const float total_force = spring_force + damping_force;

		// moving each cell
		const sf::Vector2f direction_force_A = total_force * ((posB - posA) / dist);
		cellA.accelerate(direction_force_A);

		const sf::Vector2f direction_force_B = total_force * ((posA - posB) / dist);
		cellB.accelerate(direction_force_B);
	}
};
