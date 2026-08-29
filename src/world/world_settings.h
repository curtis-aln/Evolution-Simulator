#pragma once
#include <cstdint>

struct WorldSettings
{
	inline static float bounds_radius;

	inline static uint32_t cells_x;
	inline static uint32_t cells_y;
	inline static uint8_t cell_max_capacity;

	inline static uint32_t birth_cells_x = 64;
	inline static uint32_t birth_cells_y = 64;

	inline static float border_repulsion_magnitude; // how strong it is repelled from the border
	inline static unsigned int updating_threads;

	inline static int tick_sim_multiplier;  // how many times to tick the simulation per frame, this is used to speed up the simulation when paused
	
	// Mouse Interaction
	inline static constexpr float cell_press_tollarance_factor = 1.2f; // how much bigger the cell press area is than the cell radius
	inline static constexpr float cell_drag_strength = 0.1f; // how strong the mouse drag is on a cell

	// Grid settings
	inline static constexpr float grid_line_thickness = 11.5f;
	inline static const sf::Color grid_color = { 75, 75, 75, 100 };

	inline static const float start_fading_zoom = 0.1f;
	inline static const float fade_zoom_dist = 0.02f; // grid will be fully invisible at start_fading_zoom + fade_zoom_dist
};
