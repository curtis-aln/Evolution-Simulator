#include "../Protozoa.h"
#include "../../Food/food_manager.h"
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
    energy += food->nutrients;
}

void Protozoa::reproduce_check()
{
    if (time_since_last_reproduced++ < reproductive_cooldown / m_cells_.size()) // todo
        return;

	if (stomach < m_cells_.size())
        return;

    time_since_last_reproduced = 0.f;
    stomach = 0;
    reproduce = true;
    offspring_count++;
}

void Protozoa::handle_food(FoodManager& food_manager, const bool debug)
{
    const sf::Vector2f center = get_center();
    food_manager.spatial_hash_grid.find(center.x, center.y, &nearby_food_container);

    if (debug) food_positions_nearby.clear();

	for (int i = 0; i < nearby_food_container.count; ++i)
    {
        Food* food = food_manager.at(nearby_food_container[i]);
		sf::Vector2f food_pos = food->position;

        // if the food isnt within the protozoa bounds we dont bother
        if (!m_personal_bounds_.contains(food_pos))
			continue;

        for (Cell& cell : m_cells_)
        {
            if (cell.time_since_last_ate < digestive_time)
				continue;

            const float distance_sq = (food_pos - cell.position_).lengthSquared();
			const float rad = cell.radius + FoodSettings::food_radius;
            if (distance_sq <= rad * rad)
            {
                consume(food, food_manager);
                cell.time_since_last_ate = 0;
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