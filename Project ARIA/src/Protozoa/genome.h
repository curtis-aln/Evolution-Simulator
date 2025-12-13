#pragma once

#include "../Utils/Random.h"

#include <SFML/Graphics/Color.hpp>
#include <vector>
#include <utility>
#include <cmath>
#include <unordered_map>
#include <algorithm>
#include <unordered_set>

// TODO - put the gene fetch and retrive stuff into a class
// TODO - explain the code a bit more

// Preset genetic configurations for protozoa, this defines which cells are connected by springs
struct GeneticPresets
{
    using Preset = std::vector<std::pair<int, int>>;

	// Two Cells Connected in a line
    Preset two_celled_protozoa = {
        {0, 1}
    };

	// Three Cells Connected in a triangle
    Preset three_celled_protozoa = {
        {0, 1},
        {0, 2},
        {1, 2}
    };
     
	// Five Cells Connected in a star shape with one central cell
    Preset five_celled_protozoa = {
        {0, 1},
        {0, 2},
        {0, 3},
        {0, 4}
    };

	// Collection of all presets for easy access
    std::vector<Preset> presets = {
        two_celled_protozoa,
        three_celled_protozoa,
        five_celled_protozoa
    };
};



struct GeneSettings
{
	// When we start up the simulation for the first time, we will have 75% (3 out of 4) Protozoa spawning as a preset,
	// and 25% (1 out of 4)  will be created using a custom generation algorithm.
	inline static constexpr float preset_spawn_chance = 0.75f;
    inline static const sf::Vector2i cell_amount_range = { 2, 4 }; // for the protozoa randomly generated

    // chances of adding or removing a cell per mutation event
    inline static constexpr float add_cell_chance = 0.2f;
    inline static constexpr float remove_cell_chance = 0.02f;
    
    // This determines the protozoa's mutation rate and mutation range
    inline static const sf::Vector2f init_mutation_rate_range = { 0.1f, .3f };
    inline static const sf::Vector2f init_mutation_range_range = { 0.1f, .4f };

	// This is how much the mutation rate and mutation range mutates by each generation
	inline static constexpr float delta_mutation_rate = 0.0075f; // mutation rate: chance of mutation occurring
	inline static constexpr float delta_mutation_range = 0.0125f; // mutation range: how much a gene can change when it mutates

    /* Spring Genetic Settings */
    // initial damping value for springs, determines how much velocity is lost per update
	inline static const sf::Vector2f init_damping_range = { 0.f, .1f };

	// initial spring constant for springs, determines how stiff the spring is, thus how strong the force it applies is.
    inline static const sf::Vector2f init_spring_const_range = { 0.f, .1f };

	/* The update function looks as so Extension = A * sin(Bt + C) + D
	   or in variable terms: Extension = amplitude * sin(frequency * time + offset) + vertical_shift
	   We will not constrain these values however if two cells exceed a critical distance the spring will break. // todo 
	   this is a ratio of 1 : CellDiameter, so an amplitude of 0.5 means the spring can extend or compress by half the cell diameter.
    */ // These next 4 lines just set the ranges when a random spring is made
	inline static const sf::Vector2f init_spring_amplitude_range = { 0.f, 1.f };
	inline static const sf::Vector2f init_spring_frequency_range = { 0.f, 1.f };
	inline static const sf::Vector2f init_spring_offset_range = { 0.f, 1.f };
	inline static const sf::Vector2f init_spring_vertical_shift_range = { 0.f, 1.f };

	/* Cell Genetic Settings */
    // transparency for cell and spring colors
	inline static constexpr sf::Uint8 transparency = 140;

    // Same thing happens for the cell friction sin-wave
    inline static const sf::Vector2f init_cell_amplitude_range = { 0.f, 1.f };
    inline static const sf::Vector2f init_cell_frequency_range = { 0.f, 1.f };
    inline static const sf::Vector2f init_cell_offset_range = { 0.f, 1.f };
    inline static const sf::Vector2f init_cell_vertical_shift_range = { 0.f, 1.f };

};


// This is a Gene, it contains the sin wave information for a cell or spring
struct SinWaveGene
{
    float amplitude;
    float frequency;
    float offset;
    float vertical_shift;
};

// CellGene contains the sin-wave parameters for a cell's friction modulation
struct CellGene : SinWaveGene, GeneSettings
{
    CellGene()
    {
        amplitude = Random::rand_range(init_cell_amplitude_range);
        frequency = Random::rand_range(init_cell_frequency_range);
        offset = Random::rand_range(init_cell_offset_range);
        vertical_shift = Random::rand_range(init_cell_vertical_shift_range);
    }
};

// SpringGene contains the sin-wave parameters for a spring's length modulation, as well as damping and spring constant
struct SpringGene : SinWaveGene, GeneSettings
{
    float damping;
    float spring_constant;

    SpringGene() // Randomly generate values for the spring gene
    {
    	amplitude = Random::rand_range(init_spring_amplitude_range);
    	frequency = Random::rand_range(init_spring_frequency_range);
    	offset = Random::rand_range(init_spring_offset_range);
    	vertical_shift = Random::rand_range(init_spring_vertical_shift_range);

    	damping = Random::rand_range(init_damping_range);
    	spring_constant = Random::rand_range(init_spring_const_range);
    }
};


// Genome encapsulates all evolution-related state and mutation behaviour
// previously embedded in Protozoa. It provides mutation settings, color
// mutation logic, and helper to mutate cells and springs. The caller (e.g.
// Protozoa) is responsible for acting on add/remove signals (creating or
// deleting cells) returned by mutate().
struct Genome : public GeneticPresets, public GeneSettings
{
	// Determining the mutation rate an mutation range of this protozoa
	float mutation_rate = Random::rand_range(init_mutation_rate_range);
	float mutation_range = Random::rand_range(init_mutation_range_range);

    // This tracks how old the genome is
    unsigned generation = 0u;

    // Colors for appearance (alpha will be clamped to transparency setting)
    sf::Color cell_outer_color = {200, 200, 200, transparency};
    sf::Color cell_inner_color = {180, 180, 180, transparency};
    sf::Color spring_outer_color = {220, 220, 220, transparency};
    sf::Color spring_inner_color = {200, 200, 200, transparency};


    // Now in order to create mutations for the cells and springs, we need to map their id's to their genome class.
	// Maps from unique object id -> gene data
    // Cell and spring ids live in separate id spaces and are tracked separately
    std::vector<std::optional<CellGene>> cell_genes;
    std::vector<std::optional<SpringGene>> spring_genes;


    void ensure_cell_capacity(const int id)
    {
        if (id >= cell_genes.size())
            cell_genes.resize(id + 1);
    }

    void add_cell_gene(const int id)
    {
        ensure_cell_capacity(id);
        cell_genes[id].emplace();
    }

    void remove_cell_gene(const int id)
    {
        if (id < cell_genes.size())
            cell_genes[id].reset();
    }

    CellGene* get_cell_gene(const int id)
    {
        if (id >= cell_genes.size() || !cell_genes[id])
            return nullptr;
        return &*cell_genes[id];
    }

    void ensure_spring_capacity(const int id)
    {
        if (id >= spring_genes.size())
            spring_genes.resize(id + 1);
    }

    void add_spring_gene(const int id)
    {
        ensure_spring_capacity(id);
        spring_genes[id].emplace();
    }

    void remove_spring_gene(const int id)
    {
        if (id < spring_genes.size())
            spring_genes[id].reset();
    }

    SpringGene* get_spring_gene(const int id)
    {
        if (id >= spring_genes.size() || !spring_genes[id])
            return nullptr;
        return &*spring_genes[id];
    }


    // Mutate the per-object genes (sin-wave parameters, damping, constants)
    void mutate_spring_and_cell_genes()
    {
        // Mutate cell genes
        for (auto& opt : cell_genes)
        {
            if (!opt) continue;              // skip empty slots

            CellGene& g = *opt;              // or opt.value()
            g.amplitude += Random::rand11_float() * mutation_range;
            g.frequency += Random::rand11_float() * mutation_range;
            g.offset += Random::rand11_float() * mutation_range;
            g.vertical_shift += Random::rand11_float() * mutation_range;
            // keep values within sensible ranges [0, 2*pi] for offsets, etc.
            g.offset = std::fmod(g.offset + 2.f * static_cast<float>(3.14159f), 2.f * static_cast<float>(3.14159f));
        }

        // Mutate spring genes
        for (auto& opt : spring_genes)
        {
            if (!opt) continue;              // skip empty slots

            SpringGene& g = *opt;              // or opt.value()
            g.amplitude += Random::rand11_float() * mutation_range;
            g.frequency += Random::rand11_float() * mutation_range;
            g.offset += Random::rand11_float() * mutation_range;
            g.vertical_shift += Random::rand11_float() * mutation_range;
            g.damping += Random::rand11_float() * mutation_range;
            g.spring_constant += Random::rand11_float() * mutation_range;

            // clamp spring constant and damping to sensible ranges
            g.damping = std::clamp(g.damping, init_damping_range.x, init_damping_range.y);
            g.spring_constant = std::clamp(g.spring_constant, init_spring_const_range.x, init_spring_const_range.y);
            g.offset = std::fmod(g.offset + 2.f * static_cast<float>(3.14159f), 2.f * static_cast<float>(3.14159f)); // todo
        }
    }


    // Mutate the genome and apply mutations to the provided cells and springs.
    // Returns a pair<bool,bool> indicating (should_add_cell, should_remove_cell).
    std::pair<bool, bool> mutate_genome()
    {
		// raise an exception if there are no cells or springs in the dictionaries to mutate
        if (cell_genes.empty() && spring_genes.empty())
        {
            throw std::runtime_error("Genome mutation attempted with no cells or springs present.");
		}

        // Mutate individual cells and springs
        mutate_spring_and_cell_genes();

        // Mutate colors
        cell_outer_color = mutate_color(cell_outer_color, mutation_rate);
        cell_inner_color = mutate_color(cell_inner_color, mutation_rate);
        spring_outer_color = mutate_color(spring_outer_color, mutation_rate);
        spring_inner_color = mutate_color(spring_inner_color, mutation_rate);

        // Ensure alpha matches settings
        cell_outer_color.a   = transparency;
        cell_inner_color.a   = transparency;
        spring_outer_color.a = transparency;
        spring_inner_color.a = transparency;

        // Possibly add/remove cell based on probabilities
        bool will_add = (Random::rand01_float() < add_cell_chance);
        bool will_remove = (Random::rand01_float() < remove_cell_chance);

        // Drift mutation_rate and mutation_range slightly
        mutation_rate += Random::rand11_float() * delta_mutation_rate;
        mutation_range += Random::rand11_float() * delta_mutation_range;

        return { will_add, will_remove };
    }

    // Reset genome to given settings (preserve colors if desired)
    void reset(const Genome& model_genome)
    {
        mutation_rate = model_genome.mutation_rate;
        mutation_range = model_genome.mutation_range;

        cell_outer_color   = model_genome.cell_outer_color;
        cell_inner_color = model_genome.cell_inner_color;
        spring_outer_color = model_genome.spring_outer_color;
        spring_inner_color = model_genome.spring_inner_color;

        // Replace current per-object gene dictionaries with the model's dictionaries
        cell_genes = model_genome.cell_genes;
        spring_genes = model_genome.spring_genes;

        generation = model_genome.generation;
    }

private:
    // Helper: softly clamp (keeps exploration gentle)
    static float soften_clamp(const float value, const float minv, const float maxv, const float softness = 0.15f)
    {
        if (value < minv) return minv + (value - minv) * softness;
        if (value > maxv) return maxv + (value - maxv) * softness;
        return value;
    }

    // Mutates a color in an "organic" constrained HSL space. Taken/ported from previous code.
    static sf::Color mutate_color(const sf::Color& color, const float mutation_rate = 0.002f)
    {
        float r = color.r / 255.f;
        float g = color.g / 255.f;
        float b = color.b / 255.f;

        float maxv = std::max({ r, g, b });
        float minv = std::min({ r, g, b });
        float delta = maxv - minv;

        float h = 0.f;
        float l = 0.5f * (maxv + minv);
        float s = 0.f;

        if (delta > 0.f)
        {
            s = delta / (1.f - std::abs(2.f * l - 1.f));
            if (maxv == r)
                h = 60.f * fmod(((g - b) / delta), 6.f);
            else if (maxv == g)
                h = 60.f * (((b - r) / delta) + 2.f);
            else
                h = 60.f * (((r - g) / delta) + 4.f);
            if (h < 0.f) h += 360.f;
        }

        h += Random::rand_range(-20.f, 20.f);
        s += Random::rand_range(-mutation_rate, mutation_rate);
        l += Random::rand_range(-mutation_rate, mutation_rate);

        h = fmod(h + 360.f, 360.f);
        h = soften_clamp(h, 20.f, 320.f, 0.25f);
        s = soften_clamp(s, 0.25f, 0.75f, 0.20f);
        l = soften_clamp(l, 0.35f, 0.70f, 0.20f);
        s = std::clamp(s, 0.f, 1.f);
        l = std::clamp(l, 0.f, 1.f);

        float c = (1.f - std::abs(2.f * l - 1.f)) * s;
        float hprime = h / 60.f;
        float x = c * (1.f - std::abs(fmod(hprime, 2.f) - 1.f));

        float r1 = 0.f, g1 = 0.f, b1 = 0.f;
        if (hprime < 1) { r1 = c; g1 = x; }
        else if (hprime < 2) { r1 = x; g1 = c; }
        else if (hprime < 3) { g1 = c; b1 = x; }
        else if (hprime < 4) { g1 = x; b1 = c; }
        else if (hprime < 5) { r1 = x; b1 = c; }
        else { r1 = c; b1 = x; }

        float m = l - c * 0.5f;

        return sf::Color(
            static_cast<sf::Uint8>(std::round((r1 + m) * 255.f)),
            static_cast<sf::Uint8>(std::round((g1 + m) * 255.f)),
            static_cast<sf::Uint8>(std::round((b1 + m) * 255.f)),
            color.a
        );
    }
};
