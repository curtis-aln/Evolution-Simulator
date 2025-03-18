#pragma once
#include <SFML/Graphics.hpp>
#include "../settings.h"
#include "../Utils/random.h"
#include "cell.h"

struct Spring : public GeneSettings
{
	float rest_length = Random::rand_range(init_rest_length_range);
	float damping = Random::rand_range(damping_const_range);
	float spring_constant = Random::rand_range(spring_const_range);

	sf::Color outer_color = Random::rand_val_in_vector(outer_colors);
	sf::Color inner_color = Random::rand_val_in_vector(inner_colors);

	int cell_A_id{};
	int cell_B_id{};


	// for debugging
	sf::Vector2f direction_A_force{};
	sf::Vector2f direction_B_force{};


	Spring(const int _cell_A_id, const int _cell_B_id)
		: cell_A_id(_cell_A_id), cell_B_id(_cell_B_id)
	{

	}

	void update(Cell& cell_a, Cell& cell_b, int internal_clock)
	{
		const sf::Vector2f pos_a = cell_a.position_;
		const sf::Vector2f pos_b = cell_b.position_;
		const sf::Vector2f vel_a = cell_a.velocity_;
		const sf::Vector2f vel_b = cell_b.velocity_;

		const float dist = length(pos_b - pos_a);

		// Calculating the spring force: Fs = K * (|B - A| - L)
		const float new_rest_length = sin(internal_clock * 1.f/720.f);
		const float real_rest_length = new_rest_length * rest_length_val * 5.f + rest_length_val;

		const float spring_force = spring_constant * (dist - rest_length);

		// Calculating the damping force
		const sf::Vector2f normalised_dir = ((pos_b - pos_a) / dist);
		const sf::Vector2f vel_difference = (vel_b - vel_a);
		const float damping_force = dot(normalised_dir, vel_difference) * damping;

		// Calculating total force (sum of the two forces)
		const float total_force = spring_force + damping_force;

		// moving each cell
		direction_A_force = total_force * ((pos_b - pos_a) / dist);
		cell_a.accelerate(direction_A_force);

		direction_B_force = total_force * ((pos_a - pos_b) / dist);
		cell_b.accelerate(direction_B_force);
	}
};
