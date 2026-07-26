#pragma once
#include "../Utils/random.h"

struct Range
{
    float min, max;
    float clamp(float v) const { return std::clamp(v, min, max); }
};


static struct BaseConstants
{
    static constexpr float mutation_rate_rate = 0.1f;
    static constexpr float mutation_rate_range = 0.01f;

    inline static Range init_mutation_rate_spread = { 0.4f, 0.8f };
    inline static Range init_mutation_range_spread = { 0.01f, 0.3f };
};

// Behaviour shared by every genome type
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
    // Returns val unmutated OR val + noise, clamped to limits.
    float maybe_mutate(float val, Range limits, float rate, float range) const
    {
        if (Random::rand01_float() >= rate)   // did not roll a mutation
            return val;
        return limits.clamp(val + Random::rand_range(-range, range));
    }

    // Lets mutation_rate and mutation_range drift slowly over generations.
    void mutate_meta()
    {
        auto nudge = [](float v, float range) {
            return v + Random::rand_range(-range, range);
            };

        if (Random::rand01_float() < mutation_rate_rate)
            mutation_rate = std::clamp(nudge(mutation_rate, mutation_rate_range), 0.f, 1.f);
        if (Random::rand01_float() < mutation_rate_rate)
            mutation_range = std::clamp(nudge(mutation_range, mutation_rate_range), 0.f, 1.f);
    }

    void rand_in_range(float& val, const Range& range)
    {
        Random::rand_range(range.min, range.max);
    };
};
