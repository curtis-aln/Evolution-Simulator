#pragma once
#include <SFML/Graphics.hpp>
#include "../settings.h"
#include "../Utils/random.h"
#include "cell.h"

class Spring
{
	float m_rest_length_{};
	float m_spring_constant_{};
	float m_damping_factor_{};

	// for debugging
	sf::Vector2f direction_A_force{};
	sf::Vector2f direction_B_force{};

public:
	std::pair<int, int> connection{};
	std::pair<sf::Color, sf::Color> coloring{};

	

public:
	Spring(const std::pair<int, int>& connection_ids = {}, const std::pair<sf::Color, sf::Color>& line_coloring = {},
		const float rest_length = 0, const float spring_constant = 0, const float damping_factor = 0)
	: m_rest_length_(rest_length), m_spring_constant_(spring_constant),
	m_damping_factor_(damping_factor), connection(connection_ids), coloring(line_coloring)
	{

	}

	void update(Cell& cell_a, Cell& cell_b)
	{
		const sf::Vector2f pos_a = cell_a.position_;
		const sf::Vector2f pos_b = cell_b.position_;
		const sf::Vector2f vel_a = cell_a.velocity_;
		const sf::Vector2f vel_b = cell_b.velocity_;

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
};
