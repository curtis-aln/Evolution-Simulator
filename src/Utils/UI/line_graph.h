#pragma once

#include <SFML/Graphics.hpp>
#include "../Graphics/font_renderer.hpp"
#include "../utility_SFML.h"

#include <algorithm> // Include the <algorithm> header for std::max_element


struct LineGraphSettings
{
	std::string title = "title";
	std::string x_axis_name = "x";
	std::string y_axis_name = "y";

	
	std::uint8_t transparency = 150;
	sf::Color graph_line_color = sf::Color::White;
	sf::Color under_graph_color = sf::Color::White;
	sf::Color background_color = { 40, 40, 40, transparency };
	sf::Color outline_color = { 100, 100, 100, transparency };
	sf::Color axis_color = { 50, 50, 50, transparency };

	float box_thickness = 2;
	unsigned title_font_size = 6;
	unsigned text_font_size = 12;
	unsigned stat_font_size = 10;

	std::string title_font_location = "media/fonts/Roboto-Bold.ttf";
	std::string text_font_location = "media/fonts/Roboto-Bold.ttf";


	sf::Vector2f debug_button_buffer = { 15, 15 };
	sf::Vector2f debug_button_size = { 150, 35 };

	sf::Color debug_button_default_color = { 30, 30, 30 };
	sf::Color debug_button_active_color = { 50, 50, 50 };
	sf::Color debug_button_outline_color = { 200, 200, 200 };
};


template <int maximum_data_points, int increments>
class LineGraph : LineGraphSettings
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

	// text and fonts
	Font title_font_{};
	Font text_font_{};
	Font stats_font_{};

	TextPacket x_axis_packet_{};
	TextPacket y_axis_packet_{};
	TextPacket title_packet_{};
	TextPacket min_data_packet_{};
	TextPacket max_data_packet_{};


	float m_max_data_ = 0;
	float m_min_data_ = 0;

	float m_x_axis_accumulator_ = 0.f;

	bool debug_mode_ = false;
	Button debug_toggle{};

	std::vector<sf::ConvexShape> polygons{};
	

public:
	explicit LineGraph(sf::RenderWindow* window, const sf::Rect<float> bounds = {}, const LineGraphSettings& settings = LineGraphSettings())
	: LineGraphSettings(settings), m_window_(window), m_bounds_(bounds)
	{
		polygons.resize(m_data_.size(), sf::ConvexShape(4));

		data_point_positions_.resize(m_data_.size());

		init_further_graphics();
		init_text();
		initialize_button();
		std::cout << "[info] Line graph initialized.\n";
	}

	void set_settings(const LineGraphSettings& settings)
	{
		// Assign the provided settings to the LineGraphSettings member variable
		static_cast<LineGraphSettings&>(*this) = settings;

		// You can perform any additional actions related to updating settings here
		// For example, re-initializing graphics or updating text
		init_further_graphics();
		init_text();
		initialize_button();
		std::cout << "[info] Line graph settings updated.\n";
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

		m_x_axis_accumulator_ += increments;
	}


	void update(const sf::Vector2f mouse_pos)
	{
		update_button(mouse_pos);
	}

	void set_top_left(const sf::Vector2f topleft)
	{
		m_bounds_.left = topleft.x;
		m_bounds_.top = topleft.y;
	}

	void render(Camera& camera, const bool render_absolute_position=true)
	{
		if (render_absolute_position)
		{
			camera.call_draw_at_absolute_position([&]() {
				this->render_line_graph();   // or any member function
				});
		}
		else
		{
			render_line_graph();
		}
	}



private:
	void render_line_graph()
	{
		find_max_data();
		find_data_points();

		render_bounds();

		if (!debug_mode_)
		{
			render_data_line();
			render_data_points();
		}
		else
		{
			render_area_under_graph();
		}

		render_x_axis_points();
		render_axis_bounds();
		render_text();
		render_UI();
	}

	void init_further_graphics()
	{
		// border
		m_render_rect_.setPosition(m_bounds_.position);
		m_render_rect_.setSize(m_bounds_.size);
		m_render_rect_.setFillColor(background_color);
		m_render_rect_.setOutlineColor(outline_color);
		m_render_rect_.setOutlineThickness(box_thickness);

		const sf::Vector2f size_before = m_render_rect_.getSize();
		const float extra_width = size_before.x / maximum_data_points;
		m_render_rect_.setSize({ size_before.x - extra_width, size_before.y });

		// fonts
		title_font_.set_render_window(m_window_);
		title_font_.set_font(title_font_location);
		title_font_.set_font_size(title_font_size);

		text_font_.set_render_window(m_window_);
		text_font_.set_font(text_font_location);
		text_font_.set_font_size(text_font_size);

		stats_font_.set_render_window(m_window_);
		stats_font_.set_font(text_font_location);
		stats_font_.set_font_size(stat_font_size);

		// axis
		const sf::Vector2f x_axis_size = title_font_.get_text_size(x_axis_name);
		m_x_axis_rect_.setSize(x_axis_size);

		sf::Vector2f buffer = { 0, 10 };
		const sf::Vector2f x_axis_pos = {
			m_bounds_.position.x + (m_bounds_.size.x / 2) - (x_axis_size.x / 2),
			m_bounds_.position.y + m_bounds_.size.y - x_axis_size.y - buffer.y };

		m_x_axis_rect_.setPosition(x_axis_pos);
		m_x_axis_rect_.setFillColor(axis_color);


		const sf::Vector2f y_axis_size = title_font_.get_text_size(y_axis_name);
		m_y_axis_rect_.setSize({ y_axis_size.y, y_axis_size.x }); // rotated 90 degrees
		buffer = { 10, 0 };
		const sf::Vector2f y_axis_pos = {
			m_bounds_.position.x + buffer.x,
			m_bounds_.position.y + (m_bounds_.size.y / 2) - y_axis_size.y / 2 };

		m_y_axis_rect_.setPosition(y_axis_pos);
		m_y_axis_rect_.setFillColor(axis_color);
	}


	void initialize_button()
	{
		const sf::Rect<float> rect = {
			{m_bounds_.position.x + m_bounds_.size.x - debug_button_buffer.x - debug_button_size.x,
			m_bounds_.position.y + m_bounds_.size.y - debug_button_buffer.y - debug_button_size.y},
			{debug_button_size.x, debug_button_size.y} };

		// todo 
		debug_toggle = Button(m_window_, rect);
		debug_toggle.init_font("Debug Toggle", text_font_location, sf::Color::White, text_font_size);
		debug_toggle.init_graphics(debug_button_default_color, debug_button_active_color, debug_button_outline_color, 4);
	}


	void init_text()
	{
		const sf::Vector2f padding = { 20, 20 };
		const sf::Vector2f text_size = title_font_.get_text_size(title);

		title_packet_ = {
			{ m_bounds_.position.x + m_bounds_.size.x - padding.x - text_size.x, m_bounds_.position.y + padding.y + text_size.y/2 },
			title, 0, true
		};

		min_data_packet_ = { {m_bounds_.position.x - 50, m_bounds_.position.y + m_bounds_.size.y - 10}, std::to_string(m_min_data_), 0, true };
		max_data_packet_ = { {m_bounds_.position.x - 50, m_bounds_.position.y + 10}, std::to_string(m_max_data_), 0, true };

		x_axis_packet_ = { get_center(m_x_axis_rect_), x_axis_name, 0, true };
		y_axis_packet_ = { get_center(m_y_axis_rect_), y_axis_name, -90, true };

		
	}


	void update_button(const sf::Vector2f mouse_pos)
	{
		if (debug_toggle.check_click(mouse_pos))
		{
			debug_mode_ = not debug_mode_;
		}
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


	void render_UI()
	{
		debug_toggle.draw();
	}


	void render_x_axis_points()
	{
		constexpr size_t total_points = 20;
		const float value_steps = (increments * maximum_data_points) / total_points;

		const float x_steps = m_bounds_.size.x / total_points;

		const float stat_pos = m_x_axis_accumulator_ - increments * m_data_size_;

		for (int i = 0; i < total_points; ++i)
		{
			const sf::Vector2f pos = { m_bounds_.position.x + x_steps * i, m_bounds_.position.y + m_bounds_.size.y + 30 };
			const float value = stat_pos + value_steps * i;
			stats_font_.draw(pos, std::to_string(static_cast<int>(value)), true, 30);
		}
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
		sf::VertexArray lines(sf::PrimitiveType::LineStrip, m_data_size_);
		for (int i = 0; i < m_data_size_; ++i)
		{
			lines[i].position = data_point_positions_[i];
			lines[i].color = color;
		}
		m_window_->draw(lines);

	}


	void render_area_under_graph()
	{
		if (m_data_size_ < 2)
			return;

		// Quads removed in SFML 3 — use Triangles (2 triangles per quad = 6 verts)
		sf::VertexArray vertices(sf::PrimitiveType::Triangles);
		vertices.resize((m_data_size_ - 1) * 6);

		int inc = 0;
		for (int i = 0; i < m_data_size_ - 1; ++i)
		{
			const sf::Vector2f bl = { data_point_positions_[i].x,     m_bounds_.position.y + m_bounds_.size.y };
			const sf::Vector2f br = { data_point_positions_[i + 1].x, m_bounds_.position.y + m_bounds_.size.y };
			const sf::Vector2f tr = { data_point_positions_[i + 1].x, data_point_positions_[i].y };
			const sf::Vector2f tl = data_point_positions_[i];

			// Triangle 1: bl, br, tr
			vertices[inc++].position = bl;
			vertices[inc++].position = br;
			vertices[inc++].position = tr;

			// Triangle 2: bl, tr, tl
			vertices[inc++].position = bl;
			vertices[inc++].position = tr;
			vertices[inc++].position = tl;
		}

		for (int i = 0; i < inc; ++i)
			vertices[i].color = under_graph_color;

		m_window_->draw(vertices);

		for (int i = 0; i < m_data_size_ - 1; ++i)
		{
			const sf::Vector2f pos1 = data_point_positions_[i];
			const sf::Vector2f pos2 = data_point_positions_[i + 1];

			draw_thick_line(*m_window_, pos1, { pos2.x, pos1.y }, 6, 0, graph_line_color);
			draw_thick_line(*m_window_, { pos2.x, pos1.y }, pos2, 6, 0, graph_line_color);
		}
	}


	sf::Vector2f calculate_position(const int data_index) const
	{
		const float increment_height = m_bounds_.size.y / (m_max_data_ - m_min_data_);
		const float y_pos = (m_bounds_.position.y + m_bounds_.size.y) - ((m_data_[data_index] - m_min_data_) * increment_height);

		const float x_pos = m_bounds_.position.x + (m_bounds_.size.x / m_data_.size()) * data_index;

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

		min_data_packet_.text = std::to_string(static_cast<int>(m_min_data_));
		max_data_packet_.text = std::to_string(static_cast<int>(m_max_data_));
	}


	void find_data_points()
	{
		for (int i = 0; i < m_data_size_; ++i)
		{
			data_point_positions_[i] = calculate_position(i);
		}
	}
};