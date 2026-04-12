#pragma once
#include "../random.h"
#include <SFML/Graphics/Color.hpp>


// ================================================================
// Settings
// ================================================================

struct ColorMutationSettings
{
	// Hue mutation scale — multiplied by mutation_rate to get degrees
	// Default gives ~20deg shift at mutation_rate=0.002
	float hue_mutation = 10000.f;

	// Soft biological palette bounds
	float hue_min = 20.f;
	float hue_max = 320.f;
	float saturation_min = 0.25f;
	float saturation_max = 0.75f;
	float lightness_min = 0.35f;
	float lightness_max = 0.70f;

	// How strongly values are pulled back into range [0,1]
	float saturation_softness = 0.20f;
	float lightness_softness = 0.20f;
};


// ================================================================
// HSL type
// ================================================================

struct HSL
{
	float h; // [0, 360)
	float s; // [0, 1]
	float l; // [0, 1]
};


// ================================================================
// Helpers
// ================================================================

// Pulls values gently back into range without killing exploration.
// Values outside [min, max] are nudged inward proportional to softness.
inline float soften_clamp(const float value,
	const float min_value,
	const float max_value,
	const float softness = 0.15f)
{
	if (value < min_value) return min_value + (value - min_value) * softness;
	if (value > max_value) return max_value + (value - max_value) * softness;
	return value;
}


inline HSL rgb_to_hsl(const sf::Color& c)
{
	const float r = c.r / 255.f;
	const float g = c.g / 255.f;
	const float b = c.b / 255.f;

	const float mx = std::max({ r, g, b });
	const float mn = std::min({ r, g, b });
	const float chroma = mx - mn;

	const float l = 0.5f * (mx + mn);
	const float s = (chroma < 1e-6f) ? 0.f
		: chroma / (1.f - std::abs(2.f * l - 1.f));

	float h = 0.f;
	if (chroma > 1e-6f)
	{
		if (mx == r) h = 60.f * std::fmod((g - b) / chroma, 6.f);
		else if (mx == g) h = 60.f * ((b - r) / chroma + 2.f);
		else              h = 60.f * ((r - g) / chroma + 4.f);

		if (h < 0.f) h += 360.f;
	}

	return { h, s, l };
}


inline sf::Color hsl_to_rgb(const HSL& hsl, const std::uint8_t alpha)
{
	const float chroma = (1.f - std::abs(2.f * hsl.l - 1.f)) * hsl.s;
	const float sector = hsl.h / 60.f;
	const float x = chroma * (1.f - std::abs(std::fmod(sector, 2.f) - 1.f));
	const float m = hsl.l - 0.5f * chroma;

	float r = 0.f, g = 0.f, b = 0.f;

	if (sector < 1.f) { r = chroma; g = x; }
	else if (sector < 2.f) { r = x;      g = chroma; }
	else if (sector < 3.f) { g = chroma; b = x; }
	else if (sector < 4.f) { g = x;      b = chroma; }
	else if (sector < 5.f) { r = x;                  b = chroma; }
	else { r = chroma;              b = x; }

	return {
		static_cast<std::uint8_t>((r + m) * 255.f),
		static_cast<std::uint8_t>((g + m) * 255.f),
		static_cast<std::uint8_t>((b + m) * 255.f),
		alpha
	};
}


// ================================================================
// Mutation
// ================================================================

inline sf::Color mutate_color(const sf::Color& input_color,
	const float mutation_rate = 0.002f,
	const ColorMutationSettings& settings = {})
{
	HSL hsl = rgb_to_hsl(input_color);

	// Grey inputs have no meaningful hue — randomise to avoid red drift
	if (hsl.s < 1e-4f)
		hsl.h = Random::rand_range(0.f, 360.f);

	// Mutate in perceptual HSL space
	hsl.h += Random::rand_range(-settings.hue_mutation * mutation_rate,
		settings.hue_mutation * mutation_rate);
	hsl.s += Random::rand_range(-mutation_rate, mutation_rate);
	hsl.l += Random::rand_range(-mutation_rate, mutation_rate);

	// Hue is circular — wrap cleanly
	hsl.h = std::fmod(hsl.h + 360.f, 360.f);

	// Soft-clamp S/L to biological range, then hard-clamp for safety
	hsl.s = soften_clamp(hsl.s, settings.saturation_min, settings.saturation_max, settings.saturation_softness);
	hsl.l = soften_clamp(hsl.l, settings.lightness_min, settings.lightness_max, settings.lightness_softness);
	hsl.s = std::clamp(hsl.s, 0.f, 1.f);
	hsl.l = std::clamp(hsl.l, 0.f, 1.f);

	return hsl_to_rgb(hsl, input_color.a);
}