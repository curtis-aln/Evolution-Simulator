#pragma once
#include <iostream>


struct SpringGenome
{
    float mutation_rate = 0.2f;         // chance of mutation occurring
    float mutation_range = 0.2f;
    inline static constexpr float mutation_rate_rate = 0.1;     // chance of mutation rate mutating
    inline static constexpr float mutation_rate_range = 0.01;


    int generation = 0;

    float damping = 0.1f; // velocity lost per update
    float spring_const = 0.5f; // stiffness of the spring

    // The update function looks as so Extension = A * sin(Bt + C) + D
    float amplitude = 0.5f;
    float frequency = 0.5f;
    float offset = 0.5f;
    float vertical_shift = 0.5f;
};