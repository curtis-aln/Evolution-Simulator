#pragma once
#include "GeneticNeuralNetwork.h"
#include "../Graphics/font_renderer.hpp"
#include "../UI/button.h"
#include "../utility_SFML.h"
#include "../../settings.h"


struct NodeRenderInfo
{
	sf::Vector2f pos;
	sf::Color col;
	std::pair<int, int> node_index{};
};


class NetworkRenderer : TextSettings, ButtonSettings, UI_Settings
{
	GeneticNeuralNetwork* m_network_pointer_ = nullptr;
	sf::RenderWindow* m_render_window_ = nullptr;

	sf::Rect<float> m_border_{};

	TextPacket m_title_{};
	sf::RectangleShape m_render_border_{};

	std::vector<std::vector<NodeRenderInfo>> m_node_render_info_;
	std::vector<std::pair<NodeRenderInfo*, NodeRenderInfo*>> m_connections{};

	std::vector<TextPacket> input_names{};
	std::vector<TextPacket> output_names{};

	bool debug_mode_ = false;
	Button debug_toggle{};

	sf::CircleShape circle_renderer{};

	float node_radius = 30.f;
	float link_thickness = 5.f;
	sf::Color positive_color = { 20, 240, 40 };
	sf::Color negative_color = { 240, 30, 20 };


public:
	explicit NetworkRenderer(const sf::Rect<float>& border = {}, sf::RenderWindow* render_window = nullptr, 
		GeneticNeuralNetwork* network_pointer = nullptr)
	: m_border_(border)
	{
		set_pointer(network_pointer);
		set_render_window(render_window);

		initialize_border();
		initialize_node_positions();
		initialize_synapse_lines();
		initialize_text_values();
		initialize_button();
	}


	void set_pointer(GeneticNeuralNetwork* network_pointer)
	{
		m_network_pointer_ = network_pointer;

		const std::vector<int>& size = m_network_pointer_->get_shape();
		m_node_render_info_.resize(size.size());

		for (int i = 0; i < m_node_render_info_.size(); ++i)
		{
			m_node_render_info_[i].resize(size[i]);
		}
	}

	void set_render_window(sf::RenderWindow* render_window)
	{
		m_render_window_ = render_window;
	}

	void set_title(const std::string& text)
	{
		const sf::Vector2f border_pos = m_border_.getPosition();
		const sf::Vector2f border_size = m_border_.getSize();
		const sf::Vector2f text_size = title_font.get_text_size(text);
		const sf::Vector2f position = { border_pos.x + border_size.x / 2.f, border_pos.y + text_size.y };
		
		m_title_ = { position, text, 0, true };
	}

	void set_input_node_names(const std::vector<std::string>& names)
	{
		if (names.size() != m_node_render_info_[0].size())
		{
			throw std::runtime_error("inputs need to be the same size as node inputs");
		}
		input_names.resize(names.size());

		for (int i = 0; i < names.size(); ++i)
		{
			const sf::Vector2f size = regular_font.get_text_size(names[i]);
			const sf::Vector2f pos = m_node_render_info_[0][i].pos - sf::Vector2f{ size.x + 10, 0 };
			input_names[i] = { pos, names[i], 0, true };
		}
	}

	void set_output_node_names(const std::vector<std::string>& names)
	{
		if (names.size() != m_node_render_info_[m_node_render_info_.size()-1].size())
		{
			throw std::runtime_error("outputs need to be the same size as node outputs");
		}

		output_names.resize(names.size());

		for (int i = 0; i < names.size(); ++i)
		{
			const sf::Vector2f size = regular_font.get_text_size(names[i]);
			const sf::Vector2f pos = m_node_render_info_[m_node_render_info_.size()-1][i].pos + sf::Vector2f{ size.x + 10, 0};
			output_names[i] = { pos, names[i], 0, true };
		}
	}

	void update(const sf::Vector2f mouse_pos)
	{
		update_button(mouse_pos);
	}


	void render()
	{
		render_border();
		render_synapse_lines();
		render_node_circles();
		render_text_values();
		render_UI();
	}



private:
	// initialisation
	void initialize_border()
	{
		m_render_border_.setPosition(m_border_.getPosition());
		m_render_border_.setSize(m_border_.getSize());
		m_render_border_.setFillColor(border_fill_color);
		m_render_border_.setOutlineColor(border_outline_color);
		m_render_border_.setOutlineThickness(border_outline_thickness);
		
	}

	void initialize_button()
	{
		const sf::Vector2f buffer = { 40, 40 };
		const sf::Vector2f size = { 260, 75 };
		const sf::Rect rect = {
			m_border_.left + m_border_.width - buffer.x - size.x,
			m_border_.top + m_border_.height - buffer.y - size.y,
			size.x, size.y };

		const sf::Color default_color = { 30, 30, 30 };
		const sf::Color active_color = { 50, 50, 50 };
		const sf::Color outline_color = { 200, 200, 200 };

		debug_toggle = Button(m_render_window_, rect);
		debug_toggle.init_font("Debug Toggle", regular_font_location, sf::Color::White, b_font_size);
		debug_toggle.init_graphics(default_color, active_color, outline_color, b_thickness);
	}

	void initialize_node_positions()
	{
		circle_renderer.setRadius(node_radius);
		circle_renderer.setOutlineColor(sf::Color::White);
		circle_renderer.setOutlineThickness(3);
		circle_renderer.setFillColor({ 20, 20, 20 });

		const std::vector<int> shape = m_network_pointer_->get_shape();
		const std::vector<std::vector<Node>>& neural_network = m_network_pointer_->get_neural_network();

		const float layer_sep_x = m_border_.width / (shape.size() + 1);
		std::vector<float> x_positions(shape.size());

		for (int i = 0; i < shape.size(); ++i)
		{
			x_positions[i] = m_border_.left + layer_sep_x * (i + 1);
		}

		for (int layer_idx = 0; layer_idx < neural_network.size(); ++layer_idx)
		{
			const auto num_nodes = static_cast<int>(neural_network[layer_idx].size());
			const float layer_sep_y = m_border_.height / (num_nodes + 1);

			for (int node_idx = 0; node_idx < num_nodes; ++node_idx)
			{
				const sf::Vector2f position(
					x_positions[layer_idx],
					m_border_.top + layer_sep_y * (node_idx + 1)
				);

				NodeRenderInfo& node_info = m_node_render_info_[layer_idx][node_idx];
				node_info.pos = position;
			}
		}

		for (int layer_idx = 0; layer_idx < m_node_render_info_.size(); ++layer_idx)
		{
			for (int node_idx = 0; node_idx < m_node_render_info_[layer_idx].size(); ++node_idx)
			{
				m_node_render_info_[layer_idx][node_idx].node_index = { layer_idx, node_idx };
			}
		}
	}

	void initialize_synapse_lines()
	{
		const std::vector<std::vector<Node>>& neural_network = m_network_pointer_->get_neural_network();

		for (int layer_idx = 0; layer_idx < m_node_render_info_.size(); ++layer_idx)
		{
			for (int node_idx = 0; node_idx < m_node_render_info_[layer_idx].size(); ++node_idx)
			{
				// checking if this node has connections
				const Node& node = neural_network[layer_idx][node_idx];

				for (const auto& [other_layer_idx, other_node_idx] : node.connections)
				{
					m_connections.emplace_back(
						&m_node_render_info_[layer_idx][node_idx],
						&m_node_render_info_[other_layer_idx][other_node_idx]);
				}
			}
		}
	}

	static void initialize_text_values()
	{
		
	}

	// rendering
	void render_border() const
	{
		m_render_window_->draw(m_render_border_);
		title_font.draw(m_title_);
	}

	void render_UI()
	{
		debug_toggle.draw();
	}

	void render_node_circles()
	{
		sf::CircleShape state_renderer{};

		for (std::vector<NodeRenderInfo>& layer : m_node_render_info_)
		{
			for (NodeRenderInfo& render_node : layer)
			{
				circle_renderer.setPosition(render_node.pos - sf::Vector2f{ node_radius, node_radius });
				m_render_window_->draw(circle_renderer);

				// sorting out state renderer
				const float output = m_network_pointer_->get_neural_network()[render_node.node_index.first][render_node.node_index.second].output;

				// state renderer
				const float sr_rad = std::abs(output) * node_radius;
				state_renderer.setRadius(sr_rad);

				state_renderer.setFillColor(output >= 0.f ? positive_color : negative_color);
				state_renderer.setPosition(render_node.pos - sf::Vector2f{ sr_rad, sr_rad });
				m_render_window_->draw(state_renderer);
			}
		}
	}

	void render_synapse_lines()
	{
		for (const auto& [node1_info, node2_info] : m_connections)
		{
			const float state = node_at_pair(node1_info->node_index).output;
			const sf::Color color = interpolate_colors(sf::Color::Red, sf::Color::Green, state);
			sf::Color outline_col = color;
			outline_col.a -= 50;

			draw_thick_line(*m_render_window_, node1_info->pos, node2_info->pos, 
				link_thickness, link_thickness/1.5f, color, outline_col);
		}

		if (!debug_mode_)
			return;
		
		for (const auto& [node1_info, node2_info] : m_connections)
		{
			// if debug mode we will draw the weight value of the node below the line
			const sf::Vector2f text_pos = find_point_at_distance_from_pos2(node1_info->pos, node2_info->pos, 140.f);
			const float rotation = get_line_angle(node1_info->pos, node2_info->pos);

			// nodes
			Node& node2 = node_at_pair(node2_info->node_index);

			// todo
			float state2 = 0.f;
			for (int i = 0; i < node2.connections.size(); ++i)
			{
				if (node2.connections[i] == node1_info->node_index)
				{
					state2 = std::tanh(node2.weights[i]);
				}
			}
		
			const std::string val = denary_to_str(state2, 1);
			cell_statistic_font.draw(text_pos, val, true, rotation);
		}
	}	
		
	Node& node_at_pair(const std::pair<int,int> location) const
	{
		return m_network_pointer_->at(location.first, location.second);
	}

	void render_text_values() const
	{
		// drawing inputs and outputs
		for (const TextPacket& packet : input_names)
			regular_font.draw(packet);
		for (const TextPacket& packet : output_names)
			regular_font.draw(packet);

		// if it is debug mode then we can draw values on each of the weights
		if (!debug_mode_)
			return;

		const std::vector<std::vector<Node>>& network = m_network_pointer_->get_neural_network();

		for (int layer_idx = 0; layer_idx < network.size(); ++layer_idx)
		{
			for (int node_idx = 0; node_idx < network[layer_idx].size(); ++node_idx)
			{
				const std::string val = denary_to_str(network[layer_idx][node_idx].output, 2);
				const sf::Vector2f pos = m_node_render_info_[layer_idx][node_idx].pos;
				const float text_size_y = cell_statistic_font.get_text_size(val).y;
				cell_statistic_font.draw(pos + sf::Vector2f{0, node_radius + text_size_y/2 + 8.f}, val, true);
			}
		}
	}

	// updating
	void update_button(const sf::Vector2f mouse_pos)
	{
		if (debug_toggle.check_click(mouse_pos))
		{
			debug_mode_ = not debug_mode_;
		}
	}
};