#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>

#include "../Utils/random.h"



struct CellGenome
{
    // The creation genes are genes used when a protozoa is created from scratch, such as starting up the sim or following an extinction event
    // they are roughly tuned but evolution will be the ultimate decider of what good genes are, so they don't need to be perfect.
    int generation = 0;

    // chances of adding or removing a cell per mutation event
    inline static constexpr float add_cell_chance      = 0.13f;      // chance of cell being added
    inline static constexpr float remove_cell_chance   = 0.13f;   // chance of cell being removed
    inline static constexpr float add_spring_chance    = 0.13f;      // chance of cell being added
    inline static constexpr float remove_spring_chance = 0.13f;   // chance of cell being removed

    float mutation_rate = 0.2f;         // chance of mutation occurring
    float mutation_range = 0.2f;
    inline static constexpr float mutation_rate_rate = 0.1;     // chance of mutation rate mutating
    inline static constexpr float mutation_rate_range = 0.01;

    float colour_mutation_range = 0.001f; // chance of color mutating
    inline static constexpr float radius = 90.f;
    std::uint8_t transparency = 140;

    sf::Color cell_outer_color = Random::rand_color();
	sf::Color cell_inner_color = Random::rand_color();

    // limits
    
    
	
    // max amplitude of [-2, 2], the friction value gets clamped anyway so this determines how fast it reaches from one limit to the other
    inline static constexpr float max_amplitude = 2.f;

    // // 1 second is 30 frames, so they should only be able to complete one oscillation per second (+- 1/30)
    inline static constexpr float max_frequency = 1.f / 30.f;

    // offset has a range of +- pi rad, then it loops
    inline static constexpr float max_offset = pi;

    // vertical shift minimum: -0.5 (graph fully below x axis), maximum: 0.5 (graph fully above x axis), clamping to make sure no vaue escapes [0, 1]
    inline static constexpr float max_vertical_shift = 0.5f;

    // friction sin-wave parameters, the cell's friction coefficient is determined by a sin wave with these parameters, the input being the internal clock of the protozoa
    float amplitude = 0.1f;
    float frequency = 1 / 60.f;
    float offset = 0.f;
    float vertical_shift = 0.5f;

    


    CellGenome()
    {
		cell_outer_color.a = transparency;
		cell_inner_color.a = transparency;
    }
};
