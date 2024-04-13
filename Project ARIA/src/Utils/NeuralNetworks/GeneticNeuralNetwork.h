#pragma once
#include <vector>
#include "../utility.h"
#include "../random.h"

// optimisation todo
// store outputs in a 2d vector in the main class
// allocate reserved memory for connections/
// store connections as a 2d vector in the main class
// essentially just make the node information cashe friendly by moving it all into object-of-vectors state
// input and output vectors into arrays

struct Node
{
	float output = 0.f;

	// inputs will be set by other nodes behind this node
	std::vector<float> inputs{};
	int input_counter = 0; // prevents accidental overwriting of information

	// weights are applied to the inputs to produce the output
	std::vector<float> weights{};
	float bias = 0.f;

	// a pair where the first int represents the layer, and the second int represents the index in that layer.
	std::vector<std::pair<int, int>> connections{};


	Node(const int input_size = 0, const std::vector<float>& node_weights = {}, const float node_bias = 0)
		: weights(node_weights), bias(node_bias)
	{
		inputs.resize(input_size);
		weights.resize(input_size);
	}


	void equate_output()
	{
		output = std::tanh(dot(inputs, weights) + bias);
		input_counter = 0;
	}

	// inputs should be added using this function to prevent data overwriting
	void add_input(const float value)
	{
		inputs[input_counter++] = value;
	}
};


class GeneticNeuralNetwork
{
	using layer = std::vector<Node>;

	std::vector<layer> m_network_{};


public:
	std::vector<float> inputs{};
	std::vector<float> outputs{};


public:
	GeneticNeuralNetwork(const unsigned input_size = 0, const unsigned hidden_layers = 0, const unsigned output_size = 0)
	{
		inputs.resize(input_size, 0.f);
		outputs.resize(output_size, 0.f);

		// hidden_layers + 2 includes the input & output layer
		m_network_.resize(hidden_layers + 2);
		m_network_[0].resize(input_size);
		m_network_[m_network_.size() - 1].resize(output_size);


		random_init();
	}


	void forward_propagate()
	{
		// initialising the first computational layer with inputs from the input layer.
		for (int i = 0; i < m_network_[0].size(); ++i)
		{
			m_network_[0][i].output = inputs[i];

			for (const auto& [layer, node_index] : m_network_[0][i].connections)
			{
				m_network_[layer][node_index].add_input(m_network_[0][i].output);
			}
		}

		// excluding the first layer
		for (size_t layer_index = 1; layer_index < m_network_.size(); ++layer_index)
		{
			for (Node& node : m_network_[layer_index])
			{
				forward_propagate(node);
			}
		}

		// writing the outputs
		int idx = 0;
		for (const Node& node : m_network_[m_network_.size()-1])
		{
			outputs[idx++] = node.output;
		}
	}


	std::vector<int> get_shape()
	{
		return get_shape_of_2d_vector(m_network_);
	}

	std::vector<std::vector<Node>>& get_neural_network()
	{
		return m_network_;
	}

	Node& at(const int layer_idx, const int node_idx)
	{
		return m_network_[layer_idx][node_idx];
	}


private:
	void forward_propagate(Node& node)
	{
		node.equate_output();

		// writing its output to the next nodes
		for (const auto& [layer, node_index] : node.connections)
		{
			m_network_[layer][node_index].add_input(node.output);
		}
	}

	void random_init()
	{
		// - Creates a random amount of nodes in each layer
		create_initial_nodes();

		// - Creates connections between the network
		connect_initial_nodes();

		// - for each node, gives random weight and bias values
		init_node_values();
	}

	void create_initial_nodes()
	{
		const sf::Vector2i nodes_per_layer_range = { 3, 5 };

		// excluding the input and output layer
		for (int layer_index = 1; layer_index < m_network_.size() - 1; ++layer_index)
			m_network_[layer_index].resize(Random::rand_range(nodes_per_layer_range.x, nodes_per_layer_range.y));

	}

	void connect_initial_nodes()
	{
		constexpr float add_connection_chance = 0.75f;

		// each layer in the network in decreasing order, starting from the layer before the output layer
		for (int layer_index = m_network_.size() - 2; layer_index >= 0; layer_index--)
		{
			// for each node in that layer, increasing order
			for (int node_index = 0; node_index < m_network_[layer_index].size(); ++node_index)
			{
				Node& current_node = m_network_[layer_index][node_index];

				// iterating through every single node ahead of layer_index
				for (size_t other_layer_index = layer_index + 1; other_layer_index < m_network_.size(); ++other_layer_index)
				{
					for (int other_index = 0; other_index < m_network_[other_layer_index].size(); ++other_index)
					{
						// random chance of connection being formed
						if (Random::rand01_float() < add_connection_chance)
						{
							m_network_[other_layer_index][other_index].weights.emplace_back(0.f);
							current_node.connections.emplace_back(other_layer_index, other_index);
						}
					}
				}
			}
		}
	}

	void init_node_values()
	{
		constexpr float weight_init_range = 1.f;
		constexpr float bias_init_range = 1.f;

		for (layer& Layer : m_network_)
		{
			for (Node& node : Layer)
			{
				Random::randomize_vector(node.weights, -weight_init_range, weight_init_range);
				node.bias = Random::rand_range(-bias_init_range, bias_init_range);
				node.inputs.resize(node.weights.size());
			}
		}
	}

};
