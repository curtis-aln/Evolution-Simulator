#pragma once
#include <cstdint>
#include "../../Utils/random.h"
#include "../genome_base.h"

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
    inline static float     add_spring_chance = 0.03f;
    inline static uint8_t   outer_transparency = 200;
    inline static uint8_t   inner_transparency = 100;
    inline static float     colour_mutation_range = 0.025f;

    // some mutations require a multiplier or constant to see an effect
    inline static float radius_mutation_multiplier = 5.f;
    inline static float friction_multiplier = 0.5f;
};

struct CellGenome : GenomeBase, HardConstants
{
    float radius, amplitude, frequency, offset, vertical_shift;
    uint8_t outer_r, outer_g, outer_b, inner_r, inner_g, inner_b;

    // reprductive genes
    float birth_energy_thresh = 1.0f; // as a decimal percentage
    float birth_integrity_thresh = 0.25f;
    float birth_nutrients_thresh = 0.0f;

    float offspring_energy_to_give = 0.25; // decimal percentage of energy the parent gives to its offspring
    float connective_spring_spring_const = 0.1f;
    float connective_spring_damping = 0.1f;


    CellGenome()
    {
        randomize();
    };

    void randomize()
    {
        // lambda function
        auto rand_in_range = [](float& val, const Range & range){
			val = Random::rand_range(range.min, range.max);};

		using Limit = CellInitialSpawnRanges;
        rand_in_range(amplitude, Limit::amplitude);
        rand_in_range(frequency, Limit::frequency);
        rand_in_range(offset, Limit::offset);
        rand_in_range(vertical_shift, Limit::vertical_shift);

		rand_in_range(radius, Limit::radius);

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
