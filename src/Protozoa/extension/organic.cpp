#include "../Protozoa.h"
#include "../../food_manager.h"
#include "../genetics/CellGenome.h"

#include <vector>
#include <unordered_set>


void Protozoa::record_nearby_food(Food* food)
{
    food_positions_nearby.push_back(food->position);
}

void Protozoa::consume(Food* food, FoodManager& food_manager)
{
    food_manager.remove_food(food->id);
    total_food_eaten++;
    stomach++;
    energy += 25;

    const bool ready_to_reproduce = stomach >= m_cells_.size();
    if (ready_to_reproduce)
    {
        stomach = 0;
        reproduce = true;
        offspring_count++;
    }
}

void Protozoa::handle_food(FoodManager& food_manager, const bool debug)
{
    const sf::Vector2f center = get_center();
    FixedSpan<obj_idx, FoodSettings::cell_capacity * 9> nearby = food_manager.spatial_hash_grid.find(center.x, center.y);

    if (debug) food_positions_nearby.clear();

    for (int32_t id : nearby)
    {
        Food* food = food_manager.at(id);
		sf::Vector2f food_pos = food->position;

        for (Cell& cell : m_cells_)
        {
            const float distance_sq = (food_pos - cell.position_).lengthSquared();
			const float rad = cell.radius + FoodSettings::food_radius;
            if (distance_sq <= rad * rad)
            {
                consume(food, food_manager);
                break;
            }
        }

        if (debug) record_nearby_food(food);
    }
}


void Protozoa::update_generation()
{
    for (Cell& cell : m_cells_)
    {
        cell.generation++;
    }
    for (Spring& spring : m_springs_)
    {
        spring.generation++;
	}
}

bool Protozoa::cell_wander_check(Cell& cell)
{
    const sf::Vector2f center = get_center();
    const float distance_sq = (cell.position_ - center).lengthSquared();
    
	return distance_sq > wander_threshold * wander_threshold;
}