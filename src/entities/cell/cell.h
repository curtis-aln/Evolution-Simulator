#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>
#include <cmath>
#include <algorithm>

#include "cell_settings.h"
#include "cell_genome.h"
#include "entities/body.h"

// Each organism consists of cells which work together via springs
// Each cell has their own radius and friction coefficient, as well as cosmetic factors such as color


// a faster implementation of the round function
inline float fast_round(float x)
{
	return x >= 0.0f ? floorf(x + 0.5f) : ceilf(x - 0.5f);
}

inline float fast_sqrt(float x)
{
	float y = x;

	// Initial approximation
	uint32_t i;
	std::memcpy(&i, &y, sizeof(float));

	i = (i + 0x3f800000) >> 1;

	std::memcpy(&y, &i, sizeof(float));

	// Newton refinement
	return 0.5f * (y + x / y);
}

inline float fast_sin(float x)
{
	constexpr float PI = 3.14159265358979323846f;
	constexpr float TWO_PI = 6.28318530717958647692f;

	// Wrap to [-PI, PI]
	x = x - TWO_PI * floorf((x + PI) / TWO_PI);

	// Bhaskara I approximation
	float y = (16.0f * x * (PI - fabsf(x))) /
		(5.0f * PI * PI - 4.0f * fabsf(x) * (PI - fabsf(x)));

	return y;
}


struct Cell : public CellGenome, CellSettings
{
private:
	bool reproduce_ = false; // signals to the protozoa manager that this cell needs an offspring index set
	bool dead_ = false;      // signals that the cell is in its decaying state
	bool immortal_ = false;  // cell is unaffected by death

public:
	uint32_t id_ = 0;        // The unique identifier for this cell
	uint32_t body_id_ = 0;   // Reference to the body 

	// when a spring is created between this cell and other, the springs properties
	// will be average between the two cell's spring genomes
	SpringGenome spring_genome{};

	float energy = initial_energy;

	// Stomach and food
	uint16_t time_since_last_ate_ = 0;
	uint16_t repro_timer_ = 0;
	float nutrients_ = 0.f;
	uint8_t total_food_eaten_ = 0;
	uint8_t stomach_ = 0;
	float integrity = 100.f;

	float sinwave_current_friction_ = 0.f;

	// Statistics information
	uint16_t internal_clock_ = 0;
	uint8_t  offspring_count = 0;

	std::array<uint32_t, 8> nearby_food_ids_{};
	int nearby_food_ids_size_ = 0;


public:
	Cell(const uint32_t body_id = 0) { body_id_ = body_id; }

	// ------------ Data accessors ------------
	[[nodiscard]] bool is_alive() const { return !dead_; }
	[[nodiscard]] bool can_die() const { return energy <= 0; }

	[[nodiscard]] bool should_reproduce() const { return reproduce_; }
	[[nodiscard]] bool can_reproduce() const { return energy >= repro_thresh_ && repro_timer_ >= repro_cooldown; }

	[[nodiscard]] bool should_remove() const { return can_die() && (integrity <= 0); }

	[[nodiscard]] sf::Color get_outer_color() const { return { outer_r, outer_g, outer_b, outer_transparency }; }
	[[nodiscard]] sf::Color get_inner_color() const { return { inner_r, inner_g, inner_b, inner_transparency }; }

	[[nodiscard]] sf::Vector2f get_pos_nearby(const Body* body, const float range) const;
	[[nodiscard]] float calculate_friction() const;

	// ------------ Cell functionality ------------
	void update_statistics();
	void update_organics();

	[[nodiscard]] bool eat(const float nutrients);
	void create_offspring(Cell* child, Body* parent_body, Body* child_body, const bool mutate);
	static bool consume_food_check(const sf::Vector2f& cell_pos, const sf::Vector2f& food_pos, const float combined_rad);

	void recreate();

	void turn_off_reproduction();


private:
	// Energy, Nutrient, and Integrity management
	void process_nutrients();
	void repair_integrity();
};