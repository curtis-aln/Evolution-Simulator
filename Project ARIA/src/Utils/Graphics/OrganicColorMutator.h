#pragma once
#include "../random.h"

struct ColorMutationSettings
{
	// Hue mutation (degrees)
	float hue_mutation = 20.f;

	// Soft biological palette bounds
	float hue_min = 20.f;
	float hue_max = 320.f;

	float saturation_min = 0.25f;
	float saturation_max = 0.75f;

	float lightness_min = 0.35f;
	float lightness_max = 0.70f;

	// How strongly values are pulled back into range
	float hue_softness = 0.25f;
	float saturation_softness = 0.20f;
	float lightness_softness = 0.20f;
};


inline float soften_clamp(const float value, const float min_value, const float max_value, const float softness = 0.15f)
{
	// Pulls values gently back into range without killing exploration
	if (value < min_value)
		return min_value + (value - min_value) * softness;
	if (value > max_value)
		return max_value + (value - max_value) * softness;
	return value;
}


inline sf::Color mutate_color(const sf::Color& input_color,
	const float mutation_rate = 0.002f,
	const ColorMutationSettings& settings = {})
{
	// ============================================================
	// Convert RGB [0,255] → normalized [0,1]
	// ============================================================

	const float red = input_color.r / 255.f;
	const float green = input_color.g / 255.f;
	const float blue = input_color.b / 255.f;

	const float max_channel = std::max({ red, green, blue });
	const float min_channel = std::min({ red, green, blue });
	const float chroma = max_channel - min_channel;

	// ============================================================
	// RGB → HSL
	// ============================================================

	float hue = 0.f;
	float lightness = 0.5f * (max_channel + min_channel);
	float saturation = 0.f;

	// Only compute hue/saturation for non-greys
	if (chroma > 0.f)
	{
		saturation = chroma / (1.f - std::abs(2.f * lightness - 1.f));

		if (max_channel == red)
			hue = 60.f * std::fmod((green - blue) / chroma, 6.f);
		else if (max_channel == green)
			hue = 60.f * ((blue - red) / chroma + 2.f);
		else
			hue = 60.f * ((red - green) / chroma + 4.f);

		if (hue < 0.f)
			hue += 360.f;
	}

	// ============================================================
	// Organic mutation in HSL space
	// ============================================================

	hue += Random::rand_range(-settings.hue_mutation,
		settings.hue_mutation);
	saturation += Random::rand_range(-mutation_rate, mutation_rate);
	lightness += Random::rand_range(-mutation_rate, mutation_rate);

	// Wrap hue cleanly
	hue = std::fmod(hue + 360.f, 360.f);

	// Soft biological constraints (preserve variation)
	hue = soften_clamp(hue,
		settings.hue_min,
		settings.hue_max,
		settings.hue_softness);

	saturation = soften_clamp(saturation,
		settings.saturation_min,
		settings.saturation_max,
		settings.saturation_softness);

	lightness = soften_clamp(lightness,
		settings.lightness_min,
		settings.lightness_max,
		settings.lightness_softness);

	// Hard clamp for numerical safety
	saturation = std::clamp(saturation, 0.f, 1.f);
	lightness = std::clamp(lightness, 0.f, 1.f);

	// ============================================================
	// HSL → RGB
	// ============================================================

	const float out_chroma = (1.f - std::abs(2.f * lightness - 1.f)) * saturation;
	const float hue_sector = hue / 60.f;
	const float x = out_chroma * (1.f - std::abs(std::fmod(hue_sector, 2.f) - 1.f));

	float r1 = 0.f, g1 = 0.f, b1 = 0.f;

	if (hue_sector < 1.f) { r1 = out_chroma; g1 = x; }
	else if (hue_sector < 2.f) { r1 = x; g1 = out_chroma; }
	else if (hue_sector < 3.f) { g1 = out_chroma; b1 = x; }
	else if (hue_sector < 4.f) { g1 = x; b1 = out_chroma; }
	else if (hue_sector < 5.f) { r1 = x; b1 = out_chroma; }
	else { r1 = out_chroma; b1 = x; }

	const float m = lightness - 0.5f * out_chroma;

	return {
		static_cast<sf::Uint8>((r1 + m) * 255.f),
		static_cast<sf::Uint8>((g1 + m) * 255.f),
		static_cast<sf::Uint8>((b1 + m) * 255.f),
		input_color.a
	};
}