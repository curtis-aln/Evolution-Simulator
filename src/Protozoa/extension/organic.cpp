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
    FixedSpan<obj_idx, cell_capacity * 9> nearby = food_manager.spatial_hash_grid.find(center.x, center.y);

    if (debug) food_positions_nearby.clear();

    for (int32_t id : nearby)
    {
        Food* food = food_manager.at(id);

        if (debug) record_nearby_food(food);

        if (m_personal_bounds_.contains(food->position))
            consume(food, food_manager);
    }
}


void Protozoa::mutate(const bool artificial_add_cell, const float artificial_mutatation_rate, const float artificial_mutatation_range)
{
	// First we mutate the genome, which will update the gene values for each cell and spring, as well as the colors and mutation settings
    mutate_existing_cells(artificial_mutatation_rate, artificial_mutatation_range);
    mutate_existing_springs(artificial_mutatation_rate, artificial_mutatation_range);

	// Check if we should add or remove a cell based on the genome's mutation logic
	const bool add_cell_ = Random::rand01_float() < CellGenome::add_cell_chance;
    const bool remove_cell = Random::rand01_float() < CellGenome::remove_cell_chance;

	if (add_cell_ || artificial_add_cell) 
        add_cell();

    if (remove_cell)
        remove_spring();
}

void Protozoa::mutate_existing_cells(float mut_rate, float mut_range)
{
    auto chance = [](float rate) { return Random::rand01_float() < rate; };
    auto rand_sym = [](float range) { return Random::rand_range(-range, range); };

    for (Cell& cell : m_cells_)
    {
        // if the defualt mr is zero we set it t othe cells mr
        mut_rate = (mut_rate == 0.f) ? cell.mutation_rate : mut_rate;
        mut_range = (mut_range == 0.f) ? cell.mutation_range : mut_range;

        cell.amplitude += rand_sym(mut_range) * chance(mut_rate);
        cell.frequency += rand_sym(mut_range) * chance(mut_rate);
        cell.offset += rand_sym(mut_range) * chance(mut_rate);
        cell.vertical_shift += rand_sym(mut_range) * chance(mut_rate);

        cell.mutation_rate += rand_sym(cell.mutation_rate_range) * chance(cell.mutation_rate_rate);
        cell.mutation_range += rand_sym(cell.mutation_rate_range) * chance(cell.mutation_rate_rate);
        
        if (Random::rand01_float() < cell.colour_mutation_rate)
            continue;

		cell.cell_inner_color = mutate_color(cell.cell_inner_color, cell.colour_mutation_rate);
		cell.cell_outer_color = mutate_color(cell.cell_outer_color, cell.colour_mutation_rate);
    }
}

void Protozoa::mutate_existing_springs(float mut_rate, float mut_range)
{
    auto chance = [](float rate) { return Random::rand01_float() < rate; };
    auto rand_sym = [](float range) { return Random::rand_range(-range, range); };

    for (Spring& spring : m_springs_)
    {
        // if the defualt mr is zero we set it t othe cells mr
        mut_rate = (mut_rate == 0.f) ? spring.mutation_rate : mut_rate;
        mut_range = (mut_range == 0.f) ? spring.mutation_range : mut_range;

        spring.amplitude += chance(mut_rate) ? rand_sym(mut_range) : 0.f;
        spring.frequency += chance(mut_rate) ? rand_sym(mut_range) : 0.f;
        spring.offset += chance(mut_rate) ? rand_sym(mut_range) : 0.f;
        spring.vertical_shift += chance(mut_rate) ? rand_sym(mut_range) : 0.f;

        spring.spring_const += chance(mut_rate) ? rand_sym(mut_range) : 0.f;
        spring.damping += chance(mut_rate) ? rand_sym(mut_range) : 0.f;

        spring.mutation_range += chance(spring.mutation_rate_rate) ? rand_sym(spring.mutation_rate_range) : 0.f;
        spring.mutation_rate += chance(spring.mutation_rate_rate) ? rand_sym(spring.mutation_rate_range) : 0.f;

    }
}

void Protozoa::add_cell()
{
    // finding the parent which will undergo mitosis
    const int parent_index = Random::rand_range(size_t(0), m_cells_.size() - 1);
    const Cell& parent = m_cells_[parent_index];

    sf::Vector2f position = Random::rand_pos_in_circle(parent.position_, parent.radius * 3.f);

    // creating the new cell and adding it to our cells
    Cell child = parent;
    child.id = m_cells_.size();
	child.position_ = position;
    m_cells_.push_back(child);

    // creating a spring connection to that cell
	const auto spring_id = static_cast<int>(m_springs_.size());
    Spring new_spring{ spring_id, parent_index, child.id };
    m_springs_.push_back(new_spring);
}

void Protozoa::remove_spring()
{
	// instead of removing cells, springs are removed, and cells can then get detached or "abandoned" if they have no springs connecting them to the rest of the protozoa. 
    // This is because removing cells can cause a lot of complications with ID remapping and spring connections, whereas removing a spring is simpler and more biologically plausible (like a limb falling off).
    // NOTE, TODO, this can cause protozoa to split off into two new organisms, for now we will have a simple (if the cells are too far away they die), but
	// in the future we can have them split off into a new protozoa with its own genome and everything.

    if (m_springs_.empty() || m_springs_.size() <= 1)
        return;

	const auto spring_id = Random::rand_range(size_t(0), m_springs_.size() - 1);

	// removing the spring by sweapping it with the last spring and popping back (O(1) removal)
    // TODO
}

