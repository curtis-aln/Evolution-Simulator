#pragma once
#include "../../entities/cell/cell_genome.h"

struct CellManagerSettings
{
	inline static unsigned max_protozoa;
	inline static unsigned initial_protozoa;
	inline static bool auto_reset_on_extinction;

	inline static constexpr int max_evolutionary_iterations = 5;
	inline static constexpr int desired_cell_count = 3;

	inline static constexpr int infant_time = 1000; // frames
	inline static constexpr int infant_check_interval = 20; // frames

	inline static constexpr size_t max_lifetime_samples_ = 500;
	inline static constexpr int survival_rate_window_size_ = 100;

	inline static constexpr float impulse_damage_thresh = 20.5f;
	inline static constexpr float impulse_damage_multiplier = 0.045f;

	inline static constexpr float speed_energy_tax = -0.4f;

	inline static constexpr float delta_min_speed = 1.f / 1'000'000.f;

	inline static constexpr int extincion_threshold = 10; // if there are less than this number of protozoas, we consider it an extinction event

	inline static constexpr int sim_death_immunity_frames = 400;
};