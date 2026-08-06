#pragma once
#include <cstdint>
#include <algorithm>
#include "../../Utils/random.h"

struct Range
{
    float min, max;
    float clamp(float v) const { return std::clamp(v, min, max); }
};

// ---------------------------------------------------------------------------
// Shared base: every genome type (Cell, Spring, future types) inherits this.
// ---------------------------------------------------------------------------
static struct BaseConstants
{
    static constexpr float mutation_rate_rate = 0.1f;
    static constexpr float mutation_rate_range = 0.01f;

    inline static Range init_mutation_rate_spread = { 0.4f, 0.8f };
    inline static Range init_mutation_range_spread = { 0.01f, 1.f };
};

struct GenomeBase : protected BaseConstants
{
    float mutation_rate, mutation_range;
    uint32_t generation = 0;

    GenomeBase()
    {
        rand_in_range(mutation_rate, init_mutation_rate_spread);
        rand_in_range(mutation_range, init_mutation_range_spread);
    }

protected:
    float maybe_mutate(float val, Range limits, float rate, float range) const
    {
        if (Random::rand01_float() >= rate)
            return val;
        return limits.clamp(val + Random::rand_range(-range, range));
    }

    void mutate_meta()
    {
        auto nudge = [](float v, float range) {
            return v + Random::rand_range(-range, range);
            };

        if (Random::rand01_float() < mutation_rate_rate)
            mutation_rate = nudge(mutation_rate, mutation_rate_range);
        if (Random::rand01_float() < mutation_rate_rate)
            mutation_range = nudge(mutation_range, mutation_rate_range);
    }

    void rand_in_range(float& val, const Range& range)
    {
        val = Random::rand_range(range.min, range.max);
    }
};

// ---------------------------------------------------------------------------
// Spring genome
// ---------------------------------------------------------------------------
struct SpringGeneticConstraints
{
    inline static Range amplitude = { 0.f,          1.f };
    inline static Range frequency = { -1.f / 5.f, 1.f / 5.f };
    inline static Range offset = { -3.14159f,    3.14159f };
    inline static Range vertical_shift = { -0.5f,        0.5f };
    inline static Range spring_const = { 0.f,          1.f };
    inline static Range damping = { 0.f,          1.f };
    inline static Range nutrient_transfer_rate = { -1.f,          1.f };
};

struct SpringInitialSpawnRanges
{
    inline static Range amplitude = { 0.3f,         0.5f };
    inline static Range frequency = { 1.f / 30.f,  1.f / 2.f };
    inline static Range offset = SpringGeneticConstraints::offset;
    inline static Range vertical_shift = { 0.7f,          0.7f };

    inline static Range spring_const = { 0.01f, 0.3f };
    inline static Range damping = { 0.2f, 0.8f };

    inline static Range nutrient_transfer_rate = { -0.1f, 0.5f };
};

struct SpringGenome : GenomeBase
{
    float amplitude, frequency, offset, vertical_shift;
    float nutrient_transfer_rate;
    float spring_const, damping;

    SpringGenome() { randomize(); };

    void randomize()
    {
        using Limit = SpringInitialSpawnRanges;

        rand_in_range(amplitude, Limit::amplitude);
        rand_in_range(frequency, Limit::frequency);
        rand_in_range(offset, Limit::offset);
        rand_in_range(vertical_shift, Limit::vertical_shift);

        rand_in_range(spring_const, Limit::spring_const);
        rand_in_range(damping, Limit::damping);

        rand_in_range(nutrient_transfer_rate, Limit::nutrient_transfer_rate);
    }

    void mutate(float rate = 0.f, float range = 0.f)
    {
        rate = rate > 0.f ? rate : mutation_rate;
        range = range > 0.f ? range : mutation_range;

        const auto& C = SpringGeneticConstraints{};

        amplitude = maybe_mutate(amplitude, C.amplitude, rate, range);
        frequency = maybe_mutate(frequency, C.frequency, rate, range);
        offset = maybe_mutate(offset, C.offset, rate, range);
        vertical_shift = maybe_mutate(vertical_shift, C.vertical_shift, rate, range);

        nutrient_transfer_rate = maybe_mutate(nutrient_transfer_rate, C.nutrient_transfer_rate, rate, range);

        spring_const = maybe_mutate(spring_const, C.spring_const, rate, range);
        damping = maybe_mutate(damping, C.damping, rate, range);

        mutate_meta();
    }

    void copy_genetics(const SpringGenome& parent)
    {
        amplitude = parent.amplitude;
        frequency = parent.frequency;
        offset = parent.offset;
        vertical_shift = parent.vertical_shift;

        spring_const = parent.spring_const;
        damping = parent.damping;

        nutrient_transfer_rate = parent.nutrient_transfer_rate;

        mutation_rate = parent.mutation_rate;
        mutation_range = parent.mutation_range;
    }

    void sexually_reproduce(const SpringGenome& parentA, const SpringGenome& parentB, bool cross_mutate = true)
    {
        const auto& C = SpringGeneticConstraints{};

        // Inherit mutation_rate/mutation_range the same way genes are inherited below,
        // so the rate used to mutate this child's genes reflects its own lineage.
        mutation_rate = cross_mutate
            ? (Random::rand01_float() < 0.5f ? parentA.mutation_rate : parentB.mutation_rate)
            : (parentA.mutation_rate + parentB.mutation_rate) * 0.5f;

        mutation_range = cross_mutate
            ? (Random::rand01_float() < 0.5f ? parentA.mutation_range : parentB.mutation_range)
            : (parentA.mutation_range + parentB.mutation_range) * 0.5f;

        generation = std::max(parentA.generation, parentB.generation) + 1;

        // Picks (cross_mutate) or blends (!cross_mutate) a gene from both parents, then mutates it.
        auto inherit = [&](float a, float b, Range limit) -> float {
            const float value = cross_mutate
                ? (Random::rand01_float() < 0.5f ? a : b)
                : (a + b) * 0.5f;
            return maybe_mutate(value, limit, mutation_rate, mutation_range);
            };

        amplitude = inherit(parentA.amplitude, parentB.amplitude, C.amplitude);
        frequency = inherit(parentA.frequency, parentB.frequency, C.frequency);
        offset = inherit(parentA.offset, parentB.offset, C.offset);
        vertical_shift = inherit(parentA.vertical_shift, parentB.vertical_shift, C.vertical_shift);
        spring_const = inherit(parentA.spring_const, parentB.spring_const, C.spring_const);
        damping = inherit(parentA.damping, parentB.damping, C.damping);
        nutrient_transfer_rate = inherit(parentA.nutrient_transfer_rate, parentB.nutrient_transfer_rate, C.nutrient_transfer_rate);

        mutate_meta();
    }
};

// ---------------------------------------------------------------------------
// Cell genome
// ---------------------------------------------------------------------------
struct CellGeneticConstraints
{
    inline static Range radius = { 15.f,         220.f };
    inline static Range amplitude = { -2.f,         2.f };
    inline static Range frequency = { -1.f / 1.f,  1.f / 1.f };
    inline static Range offset = { -3.14159f,    3.14159f };
    inline static Range vertical_shift = { -0.6f,        0.6f };
};

static struct CellInitialSpawnRanges
{
    inline static Range radius = { 25.f,          85.f };

    inline static Range amplitude = { 0.01f,          0.08f };
    inline static Range frequency = { 1.f / 40.f,   1.f / 2.f };
    inline static Range offset = CellGeneticConstraints::offset;
    inline static Range vertical_shift = { 0.965f,           0.99f };
};

struct HardConstants
{
    inline static float     add_cell_chance = 0.02f;
    inline static uint8_t   outer_transparency = 200;
    inline static uint8_t   inner_transparency = 100;
    inline static float     colour_mutation_range = 0.055f;

    inline static float radius_mutation_multiplier = 5.f;
    inline static float newborn_search_radius_multiplier = 5.f;
    inline static float friction_multiplier = 0.5f;
};

struct CellGenome : GenomeBase, HardConstants
{
    float radius, amplitude, frequency, offset, vertical_shift;
    uint8_t outer_r, outer_g, outer_b, inner_r, inner_g, inner_b;

    // reproductive genes
    float birth_energy_thresh = 1.0f;
    float birth_integrity_thresh = 0.25f;
    float birth_nutrients_thresh = 0.0f;

    float offspring_energy_to_give = 0.25f;
    float connective_spring_spring_const = 0.1f;
    float connective_spring_damping = 0.1f;

	float newborn_search_radius = 100.f;

    CellGenome() { randomize(); };

    void randomize()
    {
        auto rand_in_range = [](float& val, const Range& range) {
            val = Random::rand_range(range.min, range.max);
            };

        using Limit = CellInitialSpawnRanges;
        rand_in_range(amplitude, Limit::amplitude);
        rand_in_range(frequency, Limit::frequency);
        rand_in_range(offset, Limit::offset);
        rand_in_range(vertical_shift, Limit::vertical_shift);

        rand_in_range(radius, Limit::radius);
          
		rand_in_range(birth_energy_thresh, { 0.f, 1.f });
		rand_in_range(birth_integrity_thresh, { 0.f, 1.f });
		rand_in_range(birth_nutrients_thresh, { 0.f, 1.f });

		rand_in_range(offspring_energy_to_give, { 0.f, 1.f });
		rand_in_range(connective_spring_spring_const, SpringGeneticConstraints::spring_const);
		rand_in_range(connective_spring_damping, SpringGeneticConstraints::damping);

		rand_in_range(newborn_search_radius, { radius * newborn_search_radius_multiplier, radius * newborn_search_radius_multiplier * 2.f });


        outer_r = Random::rand_byte(); outer_g = Random::rand_byte(); outer_b = Random::rand_byte();
        inner_r = Random::rand_byte(); inner_g = Random::rand_byte(); inner_b = Random::rand_byte();
    }

    void mutate(float rate = 0.f, float range = 0.f)
    {
        rate = rate > 0.f ? rate : mutation_rate;
        range = range > 0.f ? range : mutation_range;

        const auto& C = CellGeneticConstraints{};

        radius = maybe_mutate(radius, C.radius, rate, range * radius_mutation_multiplier);
        amplitude = maybe_mutate(amplitude, C.amplitude, rate, range * friction_multiplier);
        frequency = maybe_mutate(frequency, C.frequency, rate, range * friction_multiplier);
        offset = maybe_mutate(offset, C.offset, rate, range * friction_multiplier);
        vertical_shift = maybe_mutate(vertical_shift, C.vertical_shift, rate, range * friction_multiplier);

		birth_energy_thresh = maybe_mutate(birth_energy_thresh, { 0.f, 1.f }, rate, range);
		birth_integrity_thresh = maybe_mutate(birth_integrity_thresh, { 0.f, 1.f }, rate, range);
		birth_nutrients_thresh = maybe_mutate(birth_nutrients_thresh, { 0.f, 1.f }, rate, range);

		offspring_energy_to_give = maybe_mutate(offspring_energy_to_give, { 0.f, 1.f }, rate, range);

		connective_spring_damping = maybe_mutate(connective_spring_damping, SpringGeneticConstraints::damping, rate, range);
		connective_spring_spring_const = maybe_mutate(connective_spring_spring_const, SpringGeneticConstraints::spring_const, rate, range);

		newborn_search_radius = maybe_mutate(newborn_search_radius, { radius * newborn_search_radius_multiplier, radius * newborn_search_radius_multiplier * 2.f }, rate, range);

        mutate_meta();
        mutate_colour(outer_r, outer_g, outer_b);
        mutate_colour(inner_r, inner_g, inner_b);
    }

    void copy_genetics(const CellGenome& parent)
    {
        amplitude = parent.amplitude;
        frequency = parent.frequency;
        offset = parent.offset;
        vertical_shift = parent.vertical_shift;
        radius = parent.radius;

        outer_r = parent.outer_r; outer_g = parent.outer_g; outer_b = parent.outer_b;
        inner_r = parent.inner_r; inner_g = parent.inner_g; inner_b = parent.inner_b;

        mutation_rate = parent.mutation_rate;
        mutation_range = parent.mutation_range;
    }

private:
    void mutate_colour(uint8_t& r, uint8_t& g, uint8_t& b) const
    {
        auto shift = [](uint8_t ch, float range) -> uint8_t {
            const int delta = static_cast<int>(Random::rand_range(-range * 255.f, range * 255.f));
            return static_cast<uint8_t>(std::clamp(static_cast<int>(ch) + delta, 0, 255));
            };
        r = shift(r, colour_mutation_range);
        g = shift(g, colour_mutation_range);
        b = shift(b, colour_mutation_range);
    }
};