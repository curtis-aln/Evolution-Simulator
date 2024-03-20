#pragma once

#include <SFML/Graphics.hpp>
#include <cmath> 
#include <chrono>

inline void drawRectOutline(const sf::Rect<float>& rect, sf::RenderWindow& window, const sf::RenderStates& renderStates)
{
	sf::VertexArray lines(sf::Lines, 8);

	// Top line
	lines[0].position = { rect.left, rect.top };
	lines[1].position = { rect.left + rect.width, rect.top };

	// Right line
	lines[2].position = { rect.left + rect.width, rect.top };
	lines[3].position = { rect.left + rect.width, rect.top + rect.height };

	// Bottom line
	lines[4].position = { rect.left + rect.width, rect.top + rect.height };
	lines[5].position = { rect.left, rect.top + rect.height };

	// Left line
	lines[6].position = { rect.left, rect.top + rect.height };
	lines[7].position = { rect.left, rect.top };

	window.draw(lines, renderStates);
}

inline void drawRectOutline(sf::Vector2f coord1, sf::Vector2f coord2, sf::RenderWindow& window, const sf::RenderStates& renderStates)
{
	sf::VertexArray lines(sf::Lines, 8);

	// Top line
	lines[0].position = coord1;
	lines[1].position = { coord2.x, coord1.y };

	// bottom line
	lines[2].position = coord2;
	lines[3].position = { coord1.x, coord2.y };

	// right line
	lines[4].position = coord2;
	lines[5].position = { coord2.x, coord1.y };

	// Left line
	lines[6].position = coord1;
	lines[7].position = { coord1.x, coord2.y };

	window.draw(lines, renderStates);
}


inline sf::VertexArray makeLine(const sf::Vector2f& point1, const sf::Vector2f& point2, const sf::Color color)
{
	sf::VertexArray line(sf::Lines, 2);
	line[0].position = point1;
	line[1].position = point2;
	line[0].color = color;
	line[1].color = color;
	return line;
}


inline std::string formatVariables(const std::vector<std::pair<std::string, double>>& variables) {
	std::ostringstream oss;
	oss.precision(2);
	oss << std::fixed;
	for (const auto& [fst, snd] : variables) {
		oss << fst << ": " << snd << ", ";
	}
	const std::string result = oss.str();
	// Remove the last comma and space
	return result.substr(0, result.size() - 2);
}

inline void displayFrameRate(sf::RenderWindow& window, const std::string& title, const int fps)
{
	std::ostringstream oss;
	oss << title << " " << fps << "fps \n";
	const std::string stringFrameRate = oss.str();
	window.setTitle(stringFrameRate);
}

inline double roundToNearestN(const double value, const unsigned decimal_places) {
	const double multiplier = pow(10, decimal_places);
	return round(value * multiplier) / multiplier;
}


inline sf::Vector2u clipToGrid(const sf::Vector2u position, const sf::Vector2u tileSize)
{
	const sf::Vector2u index(position.x / tileSize.x, position.y / tileSize.y);
	return { index.x * tileSize.x, index.y * tileSize.y };
}


inline float distSquared(const sf::Vector2f positionA, const sf::Vector2f positionB)
{
	const sf::Vector2f delta = positionB - positionA;
	return delta.x * delta.x + delta.y * delta.y;
}

inline sf::Rect<float> resizeRect(const sf::Rect<float>& rect, const sf::Vector2f resize)
{
	return {
		rect.left + resize.x,
		rect.top + resize.y,
		rect.width - resize.x * 2.f,
		rect.height - resize.y * 2.f
	};
}

inline float dot(const sf::Vector2f& v1, const sf::Vector2f& v2)
{
	return v1.x * v2.x + v1.y * v2.y;
}

inline float length(const sf::Vector2f& v)
{
	return std::sqrt(dot(v, v));
}


inline sf::Rect<float> convertCoordinates(const sf::Vector2f& v1, const sf::Vector2f& v2) {
	const float x = std::min(v1.x, v2.x);
	const float y = std::min(v1.y, v2.y);
	const float width = std::abs(v1.x - v2.x);
	const float height = std::abs(v1.y - v2.y);

	return sf::Rect<float>(x, y, width, height);
}


inline sf::VertexArray createTriangle(const float centerX, const float centerY,
	const float orientation, const float width, const float height)
{
	sf::VertexArray triangle(sf::Triangles, 3);

	// Calculate the vertices of the triangle based on the parameters
	const sf::Vector2f p1(centerX - width / 2, centerY + height / 2);
	const sf::Vector2f p2(centerX + width / 2, centerY + height / 2);
	const sf::Vector2f p3(centerX, centerY - height / 2);

	// Rotate the vertices based on the orientation
	const float cosTheta = std::cos(orientation);
	const float sinTheta = std::sin(orientation);

	triangle[0].position = {
		cosTheta * (p1.x - centerX) - sinTheta * (p1.y - centerY) + centerX,
		sinTheta * (p1.x - centerX) + cosTheta * (p1.y - centerY) + centerY
	};

	triangle[1].position = {
		cosTheta * (p2.x - centerX) - sinTheta * (p2.y - centerY) + centerX,
		sinTheta * (p2.x - centerX) + cosTheta * (p2.y - centerY) + centerY
	};

	triangle[2].position = {
		cosTheta * (p3.x - centerX) - sinTheta * (p3.y - centerY) + centerX,
		sinTheta * (p3.x - centerX) + cosTheta * (p3.y - centerY) + centerY
	};

	return triangle;
}


inline void rotateTriangle(sf::VertexArray& triangle, const float angleRadians)
{
	const sf::Vector2f pivot = (triangle[0].position + triangle[1].position + triangle[2].position) / 3.0f;

	for (size_t i = 0; i < 3; ++i)
	{
		const sf::Vector2f newPos = triangle[i].position - pivot;
		const float x = newPos.x * cos(angleRadians) - newPos.y * sin(angleRadians);
		const float y = newPos.x * sin(angleRadians) + newPos.y * cos(angleRadians);
		triangle[i].position = sf::Vector2f(x, y) + pivot;
	}
}

inline constexpr float PI = 3.14159265358979323846264338327950f;

inline sf::VertexArray createLine(const sf::Vector2f& pos1, const sf::Vector2f& pos2, const sf::Color& color = sf::Color::White)
{
	sf::VertexArray line(sf::Lines, 2);
	line[0] = { pos1, color };
	line[1] = { pos2, color };
	return line;
}

inline void normalizeAngle(float& angle_rad)
{
	while (angle_rad < 0.f)
	{
		angle_rad += 2.f * PI;
	}
	while (angle_rad >= 2.f * PI)
	{
		angle_rad -= 2.f * PI;
	}
}