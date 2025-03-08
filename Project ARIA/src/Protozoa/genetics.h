#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include "../Utils/NeuralNetworks/GeneticNeuralNetwork.h"
#include "../settings.h"
#include "../Utils/random.h"

// The Genetics class handles all of the genetic information which defines the cell and its offspring.

inline sf::Color c_rand_val_in_vector(const std::vector<sf::Color>& vals)
{
	const int idx = Random::rand_range(0, int(vals.size()) - 1);
	return vals[idx];
}

struct CellGene : GeneSettings
{
	int id{}; // Unique identifier relative to the protozoa

	sf::Color outer_color = c_rand_val_in_vector(outer_colors);
	sf::Color inner_color = c_rand_val_in_vector(inner_colors);

	float radius = CellSettings::cell_radius;
	float friction = 0.99f; // todo
};

struct SpringGene : GeneSettings
{
	float rest_length     = Random::rand_range(init_rest_length_range);
	float damping         = Random::rand_range(damping_const_range);
	float spring_constant = Random::rand_range(spring_const_range);

	sf::Color outer_color = c_rand_val_in_vector(outer_colors);
	sf::Color inner_color = c_rand_val_in_vector(inner_colors);

	int cell_A_id{};
	int cell_B_id{};
};

class ProtozoaGenes : GeneSettings
{
public:
	size_t cell_count = Random::rand_range(cell_amount_range);

	std::vector<CellGene> cell_genes;
	std::vector<SpringGene> spring_genes;

	GeneticNeuralNetwork neural_network;

public:
	ProtozoaGenes()
	{
		for (int i = 0; i < cell_count; ++i)
		{
			CellGene gene;
			gene.id = i;
			cell_genes.push_back(gene);
		}
		
		// each cell is connected to another | each cell is connected to any-one before it
		for (int i = 1; i < cell_count; ++i)
		{			
			SpringGene gene;
			gene.cell_A_id = i;
			gene.cell_B_id = Random::rand_range(0, i - 1);
			spring_genes.push_back(gene);
		}
	}
};
