#pragma once
#include <SFML/Graphics.hpp>
#include "../settings.h"
#include "cell.h"
#include "genome.h"

struct Spring
{
	int cell_A_id{};
	int cell_B_id{};

	// unique spring ID, used for genome referencing, must not change during the spring's lifetime
	int id{};

	// for debugging
	sf::Vector2f direction_A_force{};
	sf::Vector2f direction_B_force{};
	float spring_length{};
	bool broken = false;

	float breaking_length = CellSettings::cell_radius * 2.f * 3.f;
	

	Spring(const int _id, const int _cell_A_id, const int _cell_B_id)
		: cell_A_id(_cell_A_id), cell_B_id(_cell_B_id), id(_id)
	{

	}

	bool update(Cell& cell_a, Cell& cell_b, const int internal_clock, SpringGene* gene)
	{
		const sf::Vector2f pos_a = cell_a.position_;
		const sf::Vector2f pos_b = cell_b.position_;
		const sf::Vector2f vel_a = cell_a.velocity_;
		const sf::Vector2f vel_b = cell_b.velocity_;

		const float dist = length(pos_b - pos_a);
		spring_length = dist;

		// finding the rest length of the spring
		const float phase = gene->amplitude * sinf(gene->frequency * internal_clock + gene->offset) + gene->vertical_shift;
		const float rest_length = phase * CellSettings::cell_radius * 2;

		// Calculating the spring force: Fs = K * (|B - A| - L)
		const float spring_force = gene->spring_constant * (dist - rest_length);

		// Calculating the damping force
		const sf::Vector2f normalised_dir = ((pos_b - pos_a) / dist);
		const sf::Vector2f vel_difference = (vel_b - vel_a);
		const float damping_force = dot(normalised_dir, vel_difference) * gene->damping;

		// Calculating total force (sum of the two forces)
		const float total_force = spring_force + damping_force;

		// moving each cell
		direction_A_force = total_force * ((pos_b - pos_a) / dist);
		cell_a.accelerate(direction_A_force);

		direction_B_force = total_force * ((pos_a - pos_b) / dist);
		cell_b.accelerate(direction_B_force);

		// finally we check if the spring should break (if its length surpasses breaking length) and return that information
		broken = dist > breaking_length;
		return broken;
	}
};
