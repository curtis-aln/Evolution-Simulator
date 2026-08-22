#pragma once

struct CellSettings
{
	inline static float offspring_energy_cost = 50.f;

	inline static float bite_amount = 25.f;

	inline static int max_cells;
	inline static float spawn_radius;

	inline static float wander_threshold; // if a cell wanders too far away from the protozoa it kills the whole thing

	inline static size_t repro_cooldown;
	inline static float digestive_time; // per cell

	// Energy, nutrients, and integrity
	inline static float initial_energy; // energy the protozoa spawn with
	inline static float initial_integrity = 100.f;

	inline static float max_nutrients = 100;
	inline static float max_integrity = 100;
	inline static float max_energy = 100;

	inline static float integrity_conversion_rate = 0.05f;
	inline static float nutrients_conversion_rate = .185f;

	inline static constexpr float friction_energy_loss_const = 0.0095f;
	inline static const uint8_t max_cell_connections = 2;
};