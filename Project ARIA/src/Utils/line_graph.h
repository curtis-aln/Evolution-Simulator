#pragma once

#include <SFML/Graphics.hpp>


struct LineGraphSettings
{
	inline static constexpr unsigned box_thickness = 2;
	inline static constexpr sf::Uint8 transparency = 220;
	inline static const sf::Color text_background_color = { 30, 30, 30, transparency };
	inline static const sf::Color background_color = { 40, 40, 40, transparency };
	inline static const sf::Color outline_color = { 100, 100, 100, transparency };
};


class LineGraph : LineGraphSettings
{
	sf::RenderWindow* m_window_ = nullptr;

	sf::RectangleShape m_render_rect_{};
	sf::Rect<float> m_bounds_{};

	std::string title_{};
	std::string x_axis_name_{};
	std::string y_axis_name_{};

	sf::Color m_graph_line_color_{};
	sf::Color m_under_graph_color_{};


public:
	LineGraph(sf::RenderWindow* window, const sf::Rect<float> bounds = {}) : m_window_(window), m_bounds_(bounds)
	{
		// creating the render rect
		m_render_rect_.setPosition(m_bounds_.getPosition());
		m_render_rect_.setSize(m_bounds_.getSize());
		m_render_rect_.setFillColor(background_color);
		m_render_rect_.setOutlineColor(outline_color);
		m_render_rect_.setOutlineThickness(box_thickness);
	}


	void init_graphics(const std::string& title = "title", const std::string& x_axis_name = "x", const std::string& y_axis_name = "y",
		const sf::Color graph_line_color = sf::Color::White, const sf::Color under_graph_color = sf::Color::White)
	{
		title_ = title;
		x_axis_name_ = x_axis_name;
		y_axis_name_ = y_axis_name;
		m_graph_line_color_ = graph_line_color;
		m_under_graph_color_ = under_graph_color;
	}


	void add_data(sf::Vector2f data)
	{
		
	}


	void render()
	{
		m_window_->draw(m_render_rect_);
	}
};