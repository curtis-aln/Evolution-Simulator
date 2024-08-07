#pragma once

#include <random>
#include <SFML/Graphics.hpp>

inline static std::random_device dev;
inline static std::mt19937 rng{ dev() }; // random number generator
struct Random
{
	// random engines
	inline static std::uniform_real_distribution<float> float01_dist{ 0.f, 1.f };
	inline static std::uniform_real_distribution<float> float11_dist{ -1.f, 1.f };
	inline static std::uniform_int<> int01_dist{ 0, 1 };
	inline static std::uniform_int<> int11_dist{ -1, 1 };

	// basic random functions 11 = range(-1, 1), 01 = range(0, 1)
	static float rand11_float() { return float11_dist(rng); }
	static float rand01_float() { return float01_dist(rng); }
	static int   rand01_int() { return int01_dist(rng); }
	static int   rand11_int() { return int11_dist(rng); }


	// more complex random generation. specified ranges

	template <typename Type>
	static Type rand_range(const Type min, const Type max)
	{
		// Check if the Type is an integer organism_type
		if constexpr (std::is_integral_v<Type>)
		{
			std::uniform_int_distribution<Type> int_dist{ min, max };
			return int_dist(rng);
		}
		else
		{
			std::uniform_real_distribution<Type> float_dist{ min, max };
			return float_dist(rng);
		}
	}


	// random SFML::Color<Type>
	static sf::Color rand_color(const sf::Vector3<int> rgb_min = { 0, 0, 0 },
		const sf::Vector3<int> rgb_max = { 255 ,255, 255 })
	{
		return {
			static_cast<sf::Uint8>(rand_range(rgb_min.x, rgb_max.x)), // red value
			static_cast<sf::Uint8>(rand_range(rgb_min.y, rgb_max.y)), // dark_green value
			static_cast<sf::Uint8>(rand_range(rgb_min.x, rgb_max.z))  // blue value
		};
	}


	// random SFML::Vector2<Type>
	template<typename Type>
	static sf::Vector2<Type> rand_vector(const Type min, const Type max)
	{
		return { rand_range(min, max), rand_range(min, max) };
	}


	// random position within a rect
	template<typename Type>
	static sf::Vector2<Type> rand_pos_in_rect(const sf::Rect<Type>& rect)
	{
		return { rand_range(rect.left, rect.left + rect.width),
				 rand_range(rect.top, rect.top + rect.height) };
	}


	// std::vector of random points
	template<typename Type>
	static std::vector<Type> random_vector(const Type min_value, const Type max_value, const size_t size)
	{
		std::vector<Type> values;
		values.resize(size);

		for (size_t i = 0; i < size; ++i)
		{
			values[i] = rand_range(min_value, max_value);
		}

		return values;
	}

	template<typename Type>
	static void randomize_vector(std::vector<Type>& vector, const Type min_value, const Type max_value)
	{
		for (size_t i = 0; i < vector.size(); ++i)
		{
			vector[i] = rand_range(min_value, max_value);
		}
	}

	//template<typename Type>
	//static Type& choice(const std::vector<Type>& vector)
	//{
	//	return vector[Random::rand_range(0, vector.size() - 1)];
	//}
};