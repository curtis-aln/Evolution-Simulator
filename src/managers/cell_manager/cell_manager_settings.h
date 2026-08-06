#pragma once
#include "../../entities/cell/cell_genome.h"

struct CellManagerSettings
{
	inline static unsigned max_protozoa;
	inline static unsigned initial_protozoa;
	inline static bool auto_reset_on_extinction;

	inline static constexpr int max_evolutionary_iterations = 5;
	inline static constexpr int desired_cell_count = 3;

	inline static constexpr int infant_time = 400; // frames
	inline static constexpr int infant_check_interval = 100; // frames
	inline static float connection_range = CellInitialSpawnRanges::radius.max * 3.6f; // pixels
};