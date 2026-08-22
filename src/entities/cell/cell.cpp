#include "cell.h"



void Cell::recreate()
{
	energy = initial_energy;
	internal_clock_ = 0;
	generation = 0;
	time_since_last_ate_ = 0;

	nutrients_ = 0.f;
	total_food_eaten_ = 0;
	integrity = 100;
	offspring_count = 0;

	cumulative_collision_damage_ = 0.f;
	cumulative_spring_damage_ = 0.f;

	spring_genome.randomize();   // <-- clears stale genome from recycled pool slot

	reproduce_ = false;
	dead_ = false;
	immortal_ = false;
}

bool Cell::eat(const float nutrients)
{
	// This function returns false if the cell has failed to eat the food
	// and true if it has eaten the food

	if (time_since_last_ate_ < digestive_time)
		return false;

	if (nutrients > max_nutrients)
		return false;

	nutrients_ += nutrients;
	nutrients_ = std::min(nutrients_, max_nutrients);

	time_since_last_ate_ = 0;
	++total_food_eaten_;

	return true;
}

bool  Cell::consume_food_check(const sf::Vector2f& cell_pos, const sf::Vector2f& food_pos, const float combined_rad)
{
	return (food_pos - cell_pos).lengthSquared() < combined_rad * combined_rad;
}

sf::Vector2f Cell::get_pos_nearby_min_max(const sf::Vector2f parent_pos, float min_radius, float max_radius)
{
	static thread_local std::mt19937 rng{ std::random_device{}() };
	constexpr float inv_range = 1.0f / 4294967295.0f; // 1 / 2^32-1
	constexpr float two_pi = 6.28318530718f;

	const float angle = (rng() * inv_range) * two_pi;
	const float radius = min_radius + (rng() * inv_range) * (max_radius - min_radius);

	return parent_pos + sf::Vector2f(std::cos(angle) * radius, std::sin(angle) * radius);
}

void Cell::create_offspring(Body* this_body, Cell* child, Body* child_body, const bool mutate)
{
	// This function takes in blank child and child_body data and creates an offspring with it

	child_body->copy(this_body); // copies the body 
	float min_r = this_body->radius_ + child_body->radius_ + 1.f; // minimum distance between the two cells
	child_body->position_ = get_pos_nearby_min_max(this_body->position_,  min_r, min_r * 2.f); // moves the position to somewhere near the parent

	// Updating this cell's (parent)'s statistics
	repro_timer_ = 0;
	offspring_count++;

	// Updating the child cell's statistics
	child->generation = generation + 1;
	
	// a fraction of the energy of the parent is given to the child
	constexpr float energy_split = 0.25f; // fraction the parent retains; offspring gets the rest
	const float total_energy = energy;    // snapshot parent's energy before splitting

	energy = total_energy * energy_split;
	child->energy = total_energy * (1.f - energy_split);

	// Finally the genetics, we copy the cell and spring genetics and only mutate them if permissable
	child->copy_genetics(*this);
	child->spring_genome.copy_genetics(spring_genome);

	if (mutate)
	{
		child->mutate();
		child->spring_genome.mutate();
	}

	// The radius is an evolutionary paramater that needs to be told to the body
	child_body->radius_ = child->radius;

}

void Cell::turn_off_reproduction()
{
	repro_timer_ = 0;
	reproduce_ = false;
}


// When a child cell is created, it should be spawned somewhere near the parent cell.
[[nodiscard]] sf::Vector2f  Cell::get_pos_nearby(const Body* body, const float range) const
{
	// Range in terms of radii
	return Random::rand_pos_in_rect(sf::FloatRect{
		{body->position_.x - radius * range, body->position_.y - radius * range},
		{radius * range * 2 , radius * range * 2}
	});
}

[[nodiscard]] float Cell::calculate_friction() const
{
	const float sin_value = fast_sin(frequency * internal_clock_ + offset); // [-1, 1]
	const float ratio = vertical_shift + amplitude * sin_value;     // [vs-a, vs+a]
	const float clamped = std::clamp(ratio, 0.f, 1.f);
	// clamping friction to [0, 1]
	return clamped;
}

void  Cell::update_statistics()
{
	internal_clock_++;
	time_since_last_ate_++;
	repro_timer_++;
}


void Cell::update_organics()
{
	sinwave_current_friction_ = calculate_friction();

	// 1. Passive decay — base cost of being alive
	energy -= (1 - sinwave_current_friction_) * 0.0085f;

	// 2. Digest nutrients → energy, BEFORE the death check
	//    so a fed cell can survive a decay tick it otherwise couldn't
	process_nutrients();

	// 3. Death check — no energy left
	if (energy <= 0.f)
	{
		energy = 0.f;
		dead_ = true;
		return;  // dead cells don't repair or reproduce
	}

	if (integrity <= 0.f)
	{
		integrity = 0.f;
		dead_ = true;
		return;  // dead cells don't repair or reproduce
	}

	// 4. Spend energy to repair integrity
	repair_integrity();

	// 5. Flag for reproduction — use assignment so it clears itself
	//    when energy drops back below threshold
	if (check_sufficient_energy() && check_sufficient_integrity() && check_sufficient_nutrients() && check_repro_cooldown())
	{
		reproduce_ = true;
	}
}

void Cell::process_nutrients()
{
	if (nutrients_ <= 0.f)
	{
		nutrients_ = 0.f;
		return;
	}

	const float energy_capacity = max_energy - energy;
	if (energy_capacity <= 0.f)
	{
		energy = max_energy;
		return;
	}

	const float amount = std::min({ nutrients_, nutrients_conversion_rate, energy_capacity });

	energy += amount;
	nutrients_ -= amount;
}


// Renamed from update_energy — the old name was actively confusing
void Cell::repair_integrity()
{
	if (integrity >= 100.f)
		return;

	// Don't spend energy we don't have — avoids draining below 0
	// and triggering the death check on the next frame for free
	if (energy < integrity_conversion_rate)
		return;

	energy -= integrity_conversion_rate;
	integrity = std::min(100.f, integrity + integrity_conversion_rate);
}