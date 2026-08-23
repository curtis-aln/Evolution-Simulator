#pragma once
#include <SFML/System/Vector2.hpp>
#include <algorithm>

#include "../cell/cell.h"
#include "spring_settings.h"

inline static constexpr float stress_damage_const = 0.095f;

struct SpringResult { float work_done; float force_magnitude; bool broken; };

struct Spring : SpringSettings
{
	// These parameters determine the organics of the spring
	inline static float SPRING_BREAK_FORCE = 0.f;
	inline static float SPRING_BREAK_LENGTH = 0.f;
	inline static float SPRING_DAMAGE_THRESH = 0.f;
	inline static float SPRING_WORK_CONST = 0.f;

private:
	bool broken = false;

public:
	// unique spring ID, used for genome referencing, must not change during the spring's lifetime
	uint32_t id_{};

	// The springs genetic information, determined by its two connecting cells
	SpringGenome genome{};

	// we store the id's of the cells here so whe we call update in the main class we know where to look for the cells, relative to the protozoa
	uint32_t cell_A_id{};
	uint32_t cell_B_id{};

	uint16_t internal_clock_{};

	float work_done = 0.f;
	float rest_length = 0.f;
	float current_length = 0.f;
	float ratio = 0.f;

	float spring_force = {};
	float damping_force = {};

	float stress = 0.f; // 0..1, normalised force relative to break threshold

	sf::Vector2f movement_vector{ 0 ,0 };

	

	Spring(const uint8_t _id=0, const uint8_t _cell_A_id=0, const uint8_t _cell_B_id=0)
		: cell_A_id(_cell_A_id), cell_B_id(_cell_B_id), id_(_id)
	{

	}

	void reset_cell_manager()
	{
		cell_A_id = 0;
		cell_B_id = 0;

		internal_clock_ = 0;
		work_done = 0.f;
		current_length = 0.f;
		rest_length = 0.f;
		stress = 0.f;
		broken = false;
		spring_force = 0.f;
		damping_force = 0.f;
		ratio = 0.f;

		genome.frequency = 0.f;
		genome.offset = 0.f;
		genome.vertical_shift = 1.f;
		genome.amplitude = 0.f;
		genome.spring_const = 0.f;
		genome.damping = 0.f;
		genome.nutrient_transfer_rate = 0.f;   // <-- the actual leak

		movement_vector = { 0, 0 };
	}

	void break_spring()
	{
		broken = true;
	}

	bool is_spring_broken()
	{
		return broken;
	}

	void create_offspring(Spring& offspring)
	{
		offspring.genome.copy_genetics(genome);
	}

	// returns a movement vector
	void update_physics(const sf::Vector2f& pos_a, const sf::Vector2f& vel_a, const sf::Vector2f& pos_b, const sf::Vector2f& vel_b, bool immune)
	{
		internal_clock_++;

		const sf::Vector2f dir = pos_b - pos_a;
		const float length_squared = dir.x * dir.x + dir.y * dir.y;

		if (length_squared > SPRING_BREAK_LENGTH * SPRING_BREAK_LENGTH && !immune)
		{
			break_spring();
			movement_vector = { 0, 0 };
			return;
		}

		// finding the rest length of the spring
		rest_length = calculate_rest_length(internal_clock_);

		current_length = fast_sqrt(length_squared);
		const float length_diff = current_length - rest_length;
		const float inv_length = 1.0f / current_length;


		// Calculating the spring force: Fs = K * (|B - A| - L)
		spring_force = get_spring_constant() * length_diff;

		// Calculating the damping force
		const sf::Vector2f normalised_dir{ dir.x * inv_length, dir.y * inv_length};
		const sf::Vector2f vel_difference = (vel_b - vel_a);
		damping_force = normalised_dir.dot(vel_difference) * genome.damping;

		// Calculating total force (sum of the two forces)
		const float total_force = spring_force + damping_force;
		movement_vector = normalised_dir * total_force;

		// we can calculate the amount of energy this contraction / extension took
		work_done = std::abs(spring_force * length_diff);
		work_done *= SPRING_WORK_CONST;

		const float force_magnitude = std::abs(total_force);

		// Stress: 0 = relaxed, 1 = at breaking point
		stress = force_magnitude / SPRING_BREAK_FORCE;

		// Force-based break (complements your existing length-based break)
		if (force_magnitude > SPRING_BREAK_FORCE && !immune)
			break_spring();
	}

	void update_organics(Cell& cell_a, Cell& cell_b, bool is_immune)
	{
		if (!cell_a.is_alive() || !cell_b.is_alive())
		{
			broken = true;
			return;
		}

		if (stress > SPRING_DAMAGE_THRESH && !is_immune)
		{
			update_integrity(cell_a, cell_b);
		}

		transfer_nutrients(cell_a.nutrients_, cell_b.nutrients_);

		float energy_cost = -work_done / 2.f;
		cell_a.change_energy(energy_cost);
		cell_b.change_energy(energy_cost);
	}

	void update_integrity(Cell& cell_a, Cell& cell_b)
	{
		float excess = stress - SPRING_DAMAGE_THRESH;
		float damage = -excess / 2.f * stress_damage_const;

		cell_a.change_integrity(damage);
		cell_b.change_integrity(damage);
		cell_a.cumulative_spring_damage_ += abs(damage);
		cell_b.cumulative_spring_damage_ += abs(damage);
	}

private:
	[[nodiscard]] float get_spring_constant()
	{
		const float growth = std::clamp(static_cast<float>(internal_clock_) / fully_developed_age, 0.f, 1.f);
		return genome.spring_const * growth;
	}


	// takes in the nutrients of cell a and cell b and transfers between them,
// respecting max_nutrients caps and applying a fixed % loss per transfer
	void transfer_nutrients(float& nutrients_a, float& nutrients_b)
	{
		const float rate = std::max(genome.nutrient_transfer_rate, 0.0f);

		if (rate <= 0.0f)
			return;

		// figure out sender/receiver once, symmetrically
		float& sender = (nutrients_b > nutrients_a) ? nutrients_b : nutrients_a;
		float& receiver = (nutrients_b > nutrients_a) ? nutrients_a : nutrients_b;

		const float diff = sender - receiver;
		if (diff <= 0.0f)
			return; // equal, nothing to do

		const float space = CellSettings::max_nutrients - receiver;
		if (space <= 0.0f)
			return; // receiver is full, nothing can arrive anyway

		// don't send more than: the rate allows, what's needed to reach equilibrium,
		// or what the receiver can actually hold once loss is applied
		const float send_amount = std::min({ rate, sender, diff, space / (1.0f - nutrients_transfer_loss) });
		const float receive_amount = send_amount * (1.0f - nutrients_transfer_loss);

		sender -= send_amount;
		receiver += receive_amount;
	}

	float calculate_rest_length(const int internal_clock)
	{
		// sin oscillates around vertical_shift with ±amplitude swing
		const float sin_value = fast_sin(genome.frequency * internal_clock + genome.offset); // [-1, 1]
		ratio = genome.vertical_shift + genome.amplitude * sin_value;     // [vs-a, vs+a]
		const float clamped = std::clamp(ratio, 0.f, 1.f);
		return clamped * maximum_extension;
	}
};
