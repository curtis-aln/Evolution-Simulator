#pragma once
#include <SFML/Graphics.hpp>
#include "../settings.h"
#include "../Utils/random.h"
#include "cell.h"

struct Spring : public SpringGeneSettings
{
	float damping = init_damping;
	float spring_const = init_spring_const;

	float offset = Random::rand_range(offset_range);
	float frequency = Random::rand_range(frequency_range);

	sf::Color outer_color = Random::rand_color();
	sf::Color inner_color = Random::rand_color();

	int cell_A_id{};
	int cell_B_id{};

	// for debugging
	sf::Vector2f direction_A_force{};
	sf::Vector2f direction_B_force{};


	Spring(const int _cell_A_id, const int _cell_B_id)
		: cell_A_id(_cell_A_id), cell_B_id(_cell_B_id)
	{

	}

	void update(Cell& cell_a, Cell& cell_b, const int internal_clock)
	{
		const sf::Vector2f pos_a = cell_a.position_;
		const sf::Vector2f pos_b = cell_b.position_;
		const sf::Vector2f vel_a = cell_a.velocity_;
		const sf::Vector2f vel_b = cell_b.velocity_;

		const float dist = length(pos_b - pos_a);

		// finding the rest length of the spring
		const float scaled_clock = fmod(internal_clock, 360.f) * pi / 180.f;
		const float phase = (sin(frequency * scaled_clock + offset) + 1.f) / 2.f;
		const float rest_length = minLength + (phase * (maxLength - minLength));

		// Calculating the spring force: Fs = K * (|B - A| - L)
		const float spring_force = spring_const * (dist - rest_length);

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

	void call_mutate(const float mutation_rate, const float mutation_range)
	{
		if (Random::rand01_float() < mutation_rate)
		{
			offset += Random::rand11_float() * mutation_range;
			frequency += Random::rand11_float() * mutation_range;
		}
	}
};
