#pragma once
#include <iostream>


struct SpringGenome
{
    float mutation_rate = 0.2f;         // chance of mutation occurring
    float mutation_range = 0.2f;
    inline static constexpr float mutation_rate_rate = 0.1;     // chance of mutation rate mutating
    inline static constexpr float mutation_rate_range = 0.01;


    int generation = 0;

    float damping = 0.9f; // velocity lost per update
    float spring_const = 0.3f; // stiffness of the spring

    // the amplitude can range from 0, to 1 and is multiplied by a multiple of the cell radius to figure out extension
    inline static constexpr float max_amplitude = 1.f;

    // 1 second is 30 frames, so they should only be able to complete one oscillation per second (+- 1/30)
    inline static constexpr float max_frequency = 1.f / 30.f;

    // offset has a range of +- pi rad, then it loops
    inline static constexpr float max_offset = pi;

    // vertical shift minimum: -0.5 (graph fully below x axis), maximum: 0.5 (graph fully above x axis), clamping to make sure no vaue escapes [0, 1]
    inline static constexpr float max_vertical_shift = 0.5f;

    inline static constexpr float max_damping = 1.f;
    inline static constexpr float max_spring_const = 1.f;


    // The update function looks as so Extension = A * sin(Bt + C) + D
    float amplitude = 0.2f;
    float frequency = 1 / 60.f;
    float offset = 0.f;
    float vertical_shift = 0.5f;
};