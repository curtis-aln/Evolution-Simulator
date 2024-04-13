#pragma once

#include <SFML/Graphics.hpp>
#include "../fonts/font_renderer.hpp"
#include "../utility_SFML.h"

#include <algorithm> // Include the <algorithm> header for std::max_element




template <int maximum_data_points, int increments>
class LineGraph
{
	sf::RenderWindow* m_window_ = nullptr;
	sf::Rect<float> m_bounds_{};

	// data
	std::array<float, maximum_data_points> m_data_{};
	int m_data_size_ = 0;
	std::vector<sf::Vector2f> data_point_positions_{};
	

	// rendering
	sf::RectangleShape m_x_axis_rect_{};
	sf::RectangleShape m_y_axis_rect_{};
	sf::RectangleShape m_render_rect_{};

	std::string title_{};
	std::string x_axis_name_{};
	std::string y_axis_name_{};

	sf::Color m_graph_line_color_{};
	sf::Color m_under_graph_color_{};

	// text and fonts
	Font title_font_{};
	Font text_font_{};

	TextPacket x_axis_packet_{};
	TextPacket y_axis_packet_{};
	TextPacket title_packet_{};
	TextPacket min_data_packet_{};
	TextPacket max_data_packet_{};


	float m_max_data_ = 0;
	float m_min_data_ = 0;

	std::vector<sf::ConvexShape> polygons{};
	

public:
	explicit LineGraph(sf::RenderWindow* window, const sf::Rect<float> bounds = {}) : m_window_(window), m_bounds_(bounds)
	{
		polygons.resize(m_data_.size(), sf::ConvexShape(4));

		init_render_rect();
		init_text();
	}


	void init_graphics(
		const std::string& title = "m_title_", 
		const std::string& x_axis_name = "x", 
		const std::string& y_axis_name = "y",
		const sf::Color graph_line_color = sf::Color::White, 
		const sf::Color under_graph_color = sf::Color::White)
	{
		title_ = title;
		x_axis_name_ = x_axis_name;
		y_axis_name_ = y_axis_name;
		m_graph_line_color_ = graph_line_color;
		m_under_graph_color_ = under_graph_color;
	}


	void init_further_graphics(
		const unsigned box_thickness = 2,
		const sf::Uint8 transparency = 255,
		const sf::Color background_color = { 40, 40, 40, transparency },
		const sf::Color outline_color = { 100, 100, 100, transparency },
		const sf::Color axis_color = { 50, 50, 50, transparency },
		const int title_font_size = 20,
		const int text_font_size = 15,
		const std::string& title_font_location = "src/Utils/fonts/Roboto-Bold.ttf",
		const std::string& text_font_location = "src/Utils/fonts/Roboto-Bold.ttf"
	)
	{
		// border
		m_render_rect_.setPosition(m_bounds_.getPosition());
		m_render_rect_.setSize(m_bounds_.getSize());
		m_render_rect_.setFillColor(background_color);
		m_render_rect_.setOutlineColor(outline_color);
		m_render_rect_.setOutlineThickness(box_thickness);

		// fonts
		title_font_.set_render_window(m_window_);
		title_font_.set_font(title_font_location);
		title_font_.set_font_size(title_font_size);

		text_font_.set_render_window(m_window_);
		text_font_.set_font(text_font_location);
		text_font_.set_font_size(text_font_size);

		// axis
		const sf::Vector2f x_axis_size = title_font_.get_text_size(x_axis_name_);
		m_x_axis_rect_.setSize(x_axis_size);

		sf::Vector2f buffer = { 0, 10 };
		const sf::Vector2f x_axis_pos = {
			m_bounds_.left + (m_bounds_.width / 2) - (x_axis_size.x / 2),
			m_bounds_.top + m_bounds_.height - x_axis_size.y - buffer.y };

		m_x_axis_rect_.setPosition(x_axis_pos);
		m_x_axis_rect_.setFillColor(axis_color);

		const sf::Vector2f y_axis_size = title_font_.get_text_size(y_axis_name_);
		m_y_axis_rect_.setSize({ y_axis_size.y, y_axis_size.x }); // rotated 90 degrees
		buffer = { 10, 0 };
		const sf::Vector2f y_axis_pos = {
			m_bounds_.left + buffer.x,
			m_bounds_.top + (m_bounds_.height / 2) - y_axis_size.y / 2 };

		m_y_axis_rect_.setPosition(y_axis_pos);
		m_y_axis_rect_.setFillColor(axis_color);

		init_text();
	}


	void add_data(float data)
	{
		if (m_data_size_ < m_data_.size())
		{
			m_data_[m_data_size_++] = data;
		}
		else
		{
			// Shift existing data points over
			for (int i = 0; i < m_data_size_ - 1; ++i)
			{
				m_data_[i] = m_data_[i + 1];
			}

			// Add new data at the end
			m_data_[m_data_size_ - 1] = data;
		}
	}



	void render(const bool fill_bottom = false)
	{
		find_max_data();
		find_data_points();

		render_bounds();

		if (!fill_bottom)
		{
			render_data_line();
			render_data_points();
		}
		else
		{
			render_area_under_graph();
		}
		render_axis_bounds();
		render_text();
	}


private:
	void init_render_rect()
	{
		data_point_positions_.resize(m_data_.size());
	}

	void init_text()
	{
		const sf::Vector2f padding = { 20, 20 };
		const sf::Vector2f text_size = title_font_.get_text_size(title_);

		title_packet_ = {
			{ m_bounds_.left + m_bounds_.width - padding.x - text_size.x, m_bounds_.top + padding.y + text_size.y/2 },
			title_, 0, true
		};

		min_data_packet_ = { {m_bounds_.left - 20, m_bounds_.top + m_bounds_.height - 10}, std::to_string(m_min_data_), 0, true };
		max_data_packet_ = { {m_bounds_.left - 20, m_bounds_.top + 10}, std::to_string(m_max_data_), 0, true };

		x_axis_packet_ = { get_center(m_x_axis_rect_), x_axis_name_, 0, true };
		y_axis_packet_ = { get_center(m_y_axis_rect_), y_axis_name_, -90, true };

		
	}


	void render_bounds() const
	{
		m_window_->draw(m_render_rect_);
	}

	void render_axis_bounds() const
	{
		m_window_->draw(m_x_axis_rect_);
		m_window_->draw(m_y_axis_rect_);
	}
	void render_text()
	{
		title_font_.draw(title_packet_);
		text_font_.draw(x_axis_packet_);
		text_font_.draw(y_axis_packet_);
		text_font_.draw(min_data_packet_);
		text_font_.draw(max_data_packet_);
	}


	void render_data_points() const
	{
		constexpr float rad = 3;
		const sf::Color dot_color = {200, 30, 40, 200};


		sf::CircleShape dot(rad);
		dot.setFillColor(dot_color);

		for (int i = 0; i < m_data_size_; ++i)
		{
			dot.setPosition(data_point_positions_[i]  - sf::Vector2f{rad, rad});
			m_window_->draw(dot);
		}
	}

	void render_data_line(const bool fill = false) const
	{
		// Draw lines connecting consecutive data points
		if (m_data_size_ <= 1)
			return;

		const sf::Color color = { 100, 50, 200, 200 };
		sf::VertexArray lines(sf::LinesStrip, m_data_size_);
		for (int i = 0; i < m_data_size_; ++i)
		{
			lines[i].position = data_point_positions_[i];
			lines[i].color = color;
		}
		m_window_->draw(lines);

	}

	void render_area_under_graph()
	{
		sf::VertexArray vertices(sf::PrimitiveType::Quads);
		vertices.resize(m_data_size_ * 4);

		int inc = 0;
		for (int i = 0; i < m_data_size_ - 1; ++i)
		{
			vertices[inc++].position = { data_point_positions_[i].x, m_bounds_.top + m_bounds_.height }; // bottom left
			vertices[inc++].position = { data_point_positions_[i + 1].x, m_bounds_.top + m_bounds_.height }; // bottom right
			vertices[inc++].position = { data_point_positions_[i + 1].x, data_point_positions_[i].y }; // top right
			vertices[inc++].position = data_point_positions_[i]; // top left
		}

		for (int i = 0; i < inc; ++i)
		{
			vertices[i].color = m_under_graph_color_;
		}

		m_window_->draw(vertices);

		for (int i = 0; i < m_data_size_ - 1; ++i)
		{
			const sf::Vector2f pos1 = data_point_positions_[i];
			const sf::Vector2f pos2 = data_point_positions_[i + 1];

			draw_thick_line(*m_window_, pos1, { pos2.x, pos1.y },
				6, 0, m_graph_line_color_, { 0, 0, 0,0 });

			draw_thick_line(*m_window_, { pos2.x, pos1.y }, pos2,
				6, 0, m_graph_line_color_, { 0, 0, 0,0 });
		}
	}

	sf::Vector2f calculate_position(const int data_index) const
	{
		const float increment_height = m_bounds_.height / (m_max_data_ - m_min_data_);
		const float y_pos = (m_bounds_.top + m_bounds_.height) - ((m_data_[data_index] - m_min_data_) * increment_height);

		const float x_pos = m_bounds_.left + (m_bounds_.width / m_data_.size()) * data_index;

		return {x_pos, y_pos};
	}


	void find_max_data()
	{
		auto max_element = std::max_element(m_data_.begin(), m_data_.begin() + m_data_size_);
		auto min_element = std::min_element(m_data_.begin(), m_data_.begin() + m_data_size_);
		if (max_element != m_data_.end())
		{
			m_max_data_ = *max_element;
		}

		if (min_element != m_data_.end())
		{
			m_min_data_ = *min_element;
		}

		min_data_packet_.text = trim_decimal_to_string(m_min_data_, 0);
		max_data_packet_.text = trim_decimal_to_string(m_max_data_, 0);
	}

	void find_data_points()
	{
		for (int i = 0; i < m_data_size_; ++i)
		{
			data_point_positions_[i] = calculate_position(i);
		}
	}
};