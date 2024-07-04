#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include "../Utils/NeuralNetworks/GeneticNeuralNetwork.h"
#include "../settings.h"

struct CellGenetics
{
	int id;
	std::pair<sf::Color, sf::Color> colors{};
	float radius{};
};


struct SpringGenetics
{
	float starting_rest_length{};
	float damping_constant{};
	float spring_constant{};

	std::pair<sf::Color, sf::Color> colors{};
	std::pair<int, int> connecting_cell_ids{};
};



class GeneticInformation : GeneticInformationSettings
{
	std::vector<CellGenetics> cell_genetics_vector{};
	std::vector<SpringGenetics> spring_genetics_vector{};

	GeneticNeuralNetwork neural_network;


public:
	GeneticInformation()
	{
		init_random_cells();
		init_random_springs();
	}


	std::vector<CellGenetics>& get_cell_genetics() { return cell_genetics_vector; }
	std::vector<SpringGenetics>& get_spring_genetics() { return spring_genetics_vector; }



	SpringGenetics& create_cell_connection(const int cellA_id, const int cellB_id)
	{
		spring_genetics_vector.push_back({
			Random::rand_range(init_rest_length_range.x, init_rest_length_range.y),
			Random::rand_range(damping_const_range.x, damping_const_range.y),
			Random::rand_range(spring_const_range.x, spring_const_range.y),
			generate_organic_colors(),
			{cellA_id, cellB_id} });

		return spring_genetics_vector[spring_genetics_vector.size() - 1];
	}

	CellGenetics& create_cell()
	{
		const int size = cell_genetics_vector.size();
		cell_genetics_vector.emplace_back(CellGenetics(size, generate_organic_colors(),
			Random::rand_range(init_cell_radius_range.x, init_cell_radius_range.y)));

		return cell_genetics_vector[size];
	}


private:
	void init_random_cells()
	{
		const int cell_count = Random::rand_range(init_cell_count_range.x, init_cell_count_range.y);
		cell_genetics_vector.reserve(cell_count);

		for (int i = 0; i < cell_count; ++i)
			create_cell();
	}


	void init_random_springs()
	{
		// Alg: for each cell. give it a random connection to another cell in the network if
		// 1. it is not itself
		// 2. there is not already an existing connection

		for (CellGenetics& cell : cell_genetics_vector)
		{
			CellGenetics& other_cell = find_other_cell(cell);

			if (!does_connection_exist(cell, other_cell))
			{
				create_cell_connection(cell.id, other_cell.id);
			}
		}
	}


	[[nodiscard]] CellGenetics& find_other_cell(const CellGenetics& original_cell)
	{
		if (cell_genetics_vector.size() <= 1)
			throw std::runtime_error("size is <= 1");

		while (true)
		{
			CellGenetics& cell = cell_genetics_vector[Random::rand_range(size_t(0), cell_genetics_vector.size()-1)];

			if (cell.id != original_cell.id)
			{
				return cell;
			}
		}
	}

	[[nodiscard]] bool does_connection_exist(const CellGenetics& cellA, const CellGenetics& cellB) const
	{
		for (const SpringGenetics& spring : spring_genetics_vector)
		{
			const bool condA = spring.connecting_cell_ids.first == cellA.id && spring.connecting_cell_ids.second == cellB.id;
			const bool condB = spring.connecting_cell_ids.first == cellB.id && spring.connecting_cell_ids.second == cellA.id;

			if (condA || condB)
				return true;
		}
		return false;
	}


	// inline and outline color
	static std::pair<sf::Color, sf::Color> generate_organic_colors()
	{
		return {
			fill_colors[Random::rand_range(size_t(0), fill_colors.size()-1)],
			outline_colors[Random::rand_range(size_t(0), outline_colors.size()-1)]};
	}
};
