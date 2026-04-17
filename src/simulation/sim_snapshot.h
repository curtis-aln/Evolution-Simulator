#pragma once

#include "../Protozoa/cell.h"
#include "../world/world_state.h"
#include "../Protozoa/Protozoa.h"
#include "imgui/population_history.h"

struct SimSnapshot
{
    std::vector<sf::Vector2f> cell_positions;
    std::vector<sf::Color>    cell_colors;
    std::vector<float>        cell_radii;

    std::vector<sf::Vector2f> food_positions;
    std::vector<sf::Color>    food_colors;
    std::vector<float>        food_radii;

    WorldToggles toggles;
    WorldStatistics stats;
    RenderData render;

    SpatialGridData food_grid;
    SpatialGridData cell_grid;
    PopulationHistory history;

    bool selected_a_protozoa = false;
    Protozoa protozoa;
};