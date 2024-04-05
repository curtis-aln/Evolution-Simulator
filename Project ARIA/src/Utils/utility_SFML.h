#pragma once

#include <SFMl/Graphics.hpp>
#include "utility.h"

inline void draw_rect_outline(sf::Vector2f top_left, sf::Vector2f bottom_right, sf::RenderWindow& window, const sf::RenderStates& render_states = sf::RenderStates())
{
	sf::VertexArray lines(sf::Lines, 8);

	// Top line
	lines[0].position = top_left;
	lines[1].position = { bottom_right.x, top_left.y };

	// bottom line
	lines[2].position = bottom_right;
	lines[3].position = { top_left.x, bottom_right.y };

	// right line
	lines[4].position = bottom_right;
	lines[5].position = { bottom_right.x, top_left.y };

	// Left line
	lines[6].position = top_left;
	lines[7].position = { top_left.x, bottom_right.y };

	window.draw(lines, render_states);
}


inline void draw_rect_outline(const sf::Rect<float>& rect, sf::RenderWindow& window, const sf::RenderStates& render_states = sf::RenderStates())
{
	draw_rect_outline({ rect.left, rect.top }, { rect.left + rect.width, rect.top + rect.height }, window, render_states);
}


inline sf::VertexArray make_line(const sf::Vector2f& start_position, const sf::Vector2f& end_position, const sf::Color color = sf::Color::White)
{
	sf::VertexArray line(sf::Lines, 2);
	line[0] = { start_position, color };
	line[1] = { end_position, color };
	return line;
}

inline sf::Vector2u clip_to_grid(const sf::Vector2u position, const sf::Vector2u tile_size)
{
	const sf::Vector2u index(position.x / tile_size.x, position.y / tile_size.y);
	return { index.x * tile_size.x, index.y * tile_size.y };
}


inline float dist_squared(const sf::Vector2f position_a, const sf::Vector2f position_b)
{
	const sf::Vector2f delta = position_b - position_a;
	return delta.x * delta.x + delta.y * delta.y;
}

inline sf::Rect<float> resize_rect(const sf::Rect<float>& rect, const sf::Vector2f resize)
{
	return {
		rect.left + resize.x,
		rect.top + resize.y,
		rect.width - resize.x * 2.f,
		rect.height - resize.y * 2.f
	};
}

template<typename T>
sf::Vector2<T> get_rect_center(sf::Rect<T> rect)
{
	return { rect.left + rect.width / 2, rect.top + rect.height / 2 };
	
}


inline sf::Rect<float> convert_coordinates(const sf::Vector2f& v1, const sf::Vector2f& v2) {
	const float x = std::min(v1.x, v2.x);
	const float y = std::min(v1.y, v2.y);
	const float width = std::abs(v1.x - v2.x);
	const float height = std::abs(v1.y - v2.y);

	return { x, y, width, height };
}


inline sf::VertexArray create_triangle(const float center_x, const float center_y,
	const float orientation, const float width, const float height)
{
	sf::VertexArray triangle(sf::Triangles, 3);

	// Calculate the vertices of the triangle based on the parameters
	const sf::Vector2f p1(center_x - width / 2, center_y + height / 2);
	const sf::Vector2f p2(center_x + width / 2, center_y + height / 2);
	const sf::Vector2f p3(center_x, center_y - height / 2);

	// Rotate the vertices based on the orientation
	const float cos_theta = std::cos(orientation);
	const float sin_theta = std::sin(orientation);

	triangle[0].position = {
		cos_theta * (p1.x - center_x) - sin_theta * (p1.y - center_y) + center_x,
		sin_theta * (p1.x - center_x) + cos_theta * (p1.y - center_y) + center_y
	};

	triangle[1].position = {
		cos_theta * (p2.x - center_x) - sin_theta * (p2.y - center_y) + center_x,
		sin_theta * (p2.x - center_x) + cos_theta * (p2.y - center_y) + center_y
	};

	triangle[2].position = {
		cos_theta * (p3.x - center_x) - sin_theta * (p3.y - center_y) + center_x,
		sin_theta * (p3.x - center_x) + cos_theta * (p3.y - center_y) + center_y
	};

	return triangle;
}


inline void rotate_triangle(sf::VertexArray& triangle, const float angle_radians)
{
	const sf::Vector2f pivot = (triangle[0].position + triangle[1].position + triangle[2].position) / 3.0f;

	for (size_t i = 0; i < 3; ++i)
	{
		const sf::Vector2f new_pos = triangle[i].position - pivot;
		const float x = new_pos.x * cos(angle_radians) - new_pos.y * sin(angle_radians);
		const float y = new_pos.x * sin(angle_radians) + new_pos.y * cos(angle_radians);
		triangle[i].position = sf::Vector2f(x, y) + pivot;
	}
}


inline sf::VertexArray make_circle(const float radius, const sf::Vector2f center, const unsigned resolution = 100) {
	// Create a SFML vertex array to store the points of the circle
	sf::VertexArray circle(sf::LinesStrip);

	// Calculate the points around the circumference of the circle and add them to the vertex array
	for (unsigned i = 0; i <= resolution; ++i) 
	{
		const auto angle = 2 * pi * static_cast<float>(i) / static_cast<float>(resolution);
		const float x = center.x + radius * std::cos(angle);
		const float y = center.y + radius * std::sin(angle);
		circle.append(sf::Vertex(sf::Vector2f(x, y)));
	}

	return circle;
}


inline void draw_thick_line(sf::RenderWindow& window, const sf::Vector2f& point1, const sf::Vector2f& point2, 
	const float thickness, const float outline_thickness, const sf::Color& fill_color, const sf::Color& outline_color)
{
	// Calculate the length and angle of the line
	const float length = std::sqrt(dist_squared(point1, point2));
	const float angle = std::atan2(point2.y - point1.y, point2.x - point1.x) * 180 / pi;

	// Create the rectangle shape
	sf::RectangleShape line(sf::Vector2f(length, thickness));
	line.setOrigin(0, thickness / 2.0f);
	line.setPosition(point1);
	line.setRotation(angle);
	line.setFillColor(fill_color);
	line.setOutlineColor(outline_color);
	line.setOutlineThickness(outline_thickness);

	// Draw the line
	window.draw(line);
}


inline void draw_arrow(sf::RenderWindow& window, const sf::Vector2f& start_pos, const sf::Vector2f& end_pos,
	const float thickness, const float arrow_size, const sf::Color& fill_color, const sf::Color& outline_color)
{
	// Calculate the length and angle of the line
	const float angle = std::atan2(end_pos.y - start_pos.y, end_pos.x - start_pos.x);

	// Draw the line
	const sf::Vector2f shortened_end_pos(end_pos.x - thickness * std::cos(angle), end_pos.y - thickness * std::sin(angle));

	// Draw the line
	draw_thick_line(window, start_pos, shortened_end_pos, thickness, 0.f, fill_color, outline_color);

	// Calculate the tip of the arrow
	const sf::Vector2f arrow_tip(end_pos.x - arrow_size * std::cos(angle), end_pos.y - arrow_size * std::sin(angle));

	// Draw the arrowhead (triangle)
	sf::VertexArray arrow(sf::Triangles, 3);
	arrow[0].position = end_pos;
	arrow[1].position = sf::Vector2f(arrow_tip.x + arrow_size * std::cos(angle + pi / 2.f),
		arrow_tip.y + arrow_size * std::sin(angle + pi / 2.f));
	arrow[2].position = sf::Vector2f(arrow_tip.x + arrow_size * std::cos(angle - pi / 2.f),
		arrow_tip.y + arrow_size * std::sin(angle - pi / 2.f));
	for (int i = 0; i < 3; ++i) {
		arrow[i].color = fill_color;
	}
	window.draw(arrow);
}

inline void draw_direction(sf::RenderWindow& window, const sf::Vector2f& position, const sf::Vector2f& velocity, const float length,
	const float thickness, const float arrow_size, const sf::Color& fill_color, const sf::Color& outline_color)
{
	// Calculate end position based on velocity and length
	const sf::Vector2f end_pos = position + length * normalize(velocity);

	// Draw the arrow
	draw_arrow(window, position, end_pos, thickness, arrow_size, fill_color, outline_color);
}


inline sf::Vector2f get_center(const sf::RectangleShape& rect)
{
	// Get the position and size of the rectangle
	const sf::Vector2f position = rect.getPosition();
	const sf::Vector2f size = rect.getSize();

	// Calculate the center point
	const float center_x = position.x + size.x / 2;
	const float center_y = position.y + size.y / 2;

	return {center_x, center_y};
}