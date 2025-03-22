#pragma once

#include <SFML/Graphics.hpp>
#include <cmath> 
#include <chrono>

inline std::string bool_to_string(const bool val)
{
	return val ? "True" : "False";
}


inline std::string format_variables(const std::vector<std::pair<std::string, double>>& variables) {
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


inline void caption_frame_rate(sf::RenderWindow& window, const std::string& title, const int fps)
{
	std::ostringstream oss;
	oss << title << " " << fps << "fps \n";
	const std::string string_frame_rate = oss.str();
	window.setTitle(string_frame_rate);
}


template<typename T>
T round_to_nearest_n(const T value, const unsigned decimal_places) {
	const T multiplier = static_cast<T>(pow(10, decimal_places));
	return round(value * multiplier) / multiplier;
}


template<typename T>
std::string denary_to_str(T number, const size_t precision, const bool round=true)
{
	if (round)
		number = round_to_nearest_n(number, static_cast<unsigned>(precision));

	// Convert the double to a string with the specified precision
	std::string result = std::to_string(number);

	if (const size_t dot_position = result.find('.'); dot_position != std::string::npos)
	{
		// Check if there are enough characters after the dot
		if (result.length() - dot_position > precision + 1)
		{
			result = result.substr(0, dot_position + precision + 1);
		}
	}

	return result;
}


template<typename T>
T dot(const sf::Vector2<T>& v1, const sf::Vector2<T>& v2)
{
	return v1.x * v2.x + v1.y * v2.y;
}

template<typename T>
T dot(const std::vector<T>& vector1, const std::vector<T>& vector2)
{
	if (vector1.size() != vector2.size())
		throw std::runtime_error("vectors need to be the same size");

	T result = 0.f;
	for (size_t i = 0; i < vector1.size(); ++i)
	{
		result += vector1[i] * vector2[i];
	}
	return result;
}


template<typename T>
T length(const sf::Vector2<T>& v)
{
	return std::sqrt(dot(v, v));
}


inline constexpr float pi = 3.14159265358979323846264338327950f;


inline void normalize_angle(float& angle_radians)
{
	while (angle_radians < 0.f)
	{
		angle_radians += 2.f * pi;
	}
	while (angle_radians >= 2.f * pi)
	{
		angle_radians -= 2.f * pi;
	}
}


inline sf::Vector2f normalize(const sf::Vector2f& vector)
{
	if (const float length = std::sqrt(vector.x * vector.x + vector.y * vector.y); length != 0)
		return {vector.x / length, vector.y / length};
	
	return {0, 0};
}


template<typename T>
std::vector<int> get_shape_of_2d_vector(std::vector<std::vector<T>>& container)
{
	std::vector<int> shape;
	shape.resize(container.size());

	for (int i = 0; i < container.size(); ++i)
	{
		shape[i] = static_cast<int>(container[i].size());
	}

	return shape;
}
