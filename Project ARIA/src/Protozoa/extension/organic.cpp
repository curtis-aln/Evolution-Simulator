#include "../Protozoa.h"
#include "../../food_manager.h"


inline sf::Color mutate_color(const sf::Color& color, float mutation_rate = 0.012f)
{
    // Convert RGB to HSL
    float r = color.r / 255.f;
    float g = color.g / 255.f;
    float b = color.b / 255.f;

    float max_val = std::max({ r, g, b });
    float min_val = std::min({ r, g, b });
    float delta = max_val - min_val;

    float h = 0.f;
    float s = (max_val == 0) ? 0.f : (delta / max_val);
    float l = (max_val + min_val) / 2.f;

    // Calculate hue
    if (delta != 0)
    {
        if (max_val == r)      h = 60.f * (fmod((g - b) / delta, 6));
        else if (max_val == g) h = 60.f * ((b - r) / delta + 2);
        else if (max_val == b) h = 60.f * ((r - g) / delta + 4);
    }
    if (h < 0) h += 360.f;

    // Mutate hue, saturation, and lightness
    h += Random::rand_range(-30.f, 30.f); // Larger hue mutations
    s += Random::rand_range(-mutation_rate, mutation_rate);
    l += Random::rand_range(-mutation_rate, mutation_rate);

    h = fmod(h + 360.f, 360.f); // Keep hue in range
    s = std::clamp(s, 0.2f, 1.0f); // Prevent low saturation (dull colors)
    l = std::clamp(l, 0.2f, 0.8f); // Prevent black or white

    // Convert back to RGB
    float c = (1 - std::abs(2 * l - 1)) * s;
    float x = c * (1 - std::abs(fmod(h / 60.f, 2) - 1));
    float m = l - c / 2;

    float r1, g1, b1;
    if (h < 60) { r1 = c; g1 = x; b1 = 0; }
    else if (h < 120) { r1 = x; g1 = c; b1 = 0; }
    else if (h < 180) { r1 = 0; g1 = c; b1 = x; }
    else if (h < 240) { r1 = 0; g1 = x; b1 = c; }
    else if (h < 300) { r1 = x; g1 = 0; b1 = c; }
    else { r1 = c; g1 = 0; b1 = x; }

    return sf::Color((r1 + m) * 255, (g1 + m) * 255, (b1 + m) * 255);
}


void Protozoa::handle_food(FoodManager& food_manager)
{
	const sf::Vector2f center = get_center();
	c_Vec<food_manager.spatial_hash_grid.max_nearby_capacity>& nearby =
		food_manager.spatial_hash_grid.find(center);

	food_positions_nearby.clear();

	for (int i = 0; i < nearby.size; ++i)
	{
		Food* food_particle = food_manager.at(nearby.at(i));
		food_positions_nearby.push_back(food_particle->position);

		if (m_personal_bounds_.contains(food_particle->position)) // todo better collision handling
		{
			food_manager.remove_food(food_particle->id);
			reproduce = true;
		}
	}
}



void Protozoa::mutate()
{
	// mutating the cells in this organism
	for (Cell& cell : m_cells_)
	{
        cell.call_mutate();

        cell.inner_color = mutate_color(cell.inner_color);
        cell.outer_color = mutate_color(cell.outer_color);
	}

	// mutating the springs in this organism
	for (Spring& spring : m_springs_)
	{
        spring.call_mutate();

        spring.inner_color = mutate_color(spring.inner_color);
        spring.outer_color = mutate_color(spring.outer_color);
	}

    // adding new components to the organism
}