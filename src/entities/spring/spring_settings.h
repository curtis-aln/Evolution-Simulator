#pragma once

struct SpringSettings
{
	inline static float maximum_extension;

	inline static float spring_work_const; // how we scale the energy cost of springs
	inline static float spring_break_force = 35.f;
	inline static float spring_break_length_factor = 1.5f;
	inline static float spring_damage_threshold = 30.5f;
	
	inline static constexpr float fully_developed_age = 300.f; // arbitrary age at which the spring is considered fully developed
	inline static constexpr float nutrients_transfer_loss = 0.04f;

	inline static constexpr float stress_damage_const = 0.095f;

};