#include "../Protozoa.h"
#include "../../food_manager.h"

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


void Protozoa::mutate()
{
	// First we mutate the genome, which will update the gene values for each cell and spring, as well as the colors and mutation settings
	mutate_genome();

	// Check if we should add or remove a cell based on the genome's mutation logic
    auto [add_cell_prob, remove_cell_prob] = get_add_remove_cell_signals();

    if (add_cell_prob)   add_cell();
    if (remove_cell_prob) remove_spring();
}

void Protozoa::add_cell()
{
    // finding the parent which will undergo mitosis
    const int parent_index = Random::rand_range(size_t(0), m_cells_.size() - 1);
    sf::Vector2f position = Random::rand_pos_in_circle(m_cells_[parent_index].position_, m_cells_[parent_index].radius);

    // creating the new cell and adding it to our cells
    const int cell_id = m_cells_.size();
    Cell new_cell{ cell_id, position};
    m_cells_.push_back(new_cell);

    // creating a spring connection to that cell
	const auto spring_id = static_cast<int>(m_springs_.size());
    Spring new_spring{ spring_id, parent_index, cell_id };
    m_springs_.push_back(new_spring);

	// Now we need to update our genome dictionaries to account for the new cell and spring
    add_cell_gene(cell_id);
	add_spring_gene(spring_id);

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


void Protozoa::load_preset(Preset& preset, sf::Vector2f position)
{
    /*
	The load preset function initializes the protozoa's cells and springs with a pre-defined structure.
	The preset is a list of connections between cell IDs, where each connection represents a spring between two cells.
	If a position is provided, the protozoa will be spawned around that position instead of a random location.
    We can simply create genetic variation by choosing a small percentage of these cells to undergo some mutation cycles
     */

    // Clear existing genetic data Just In Case
    m_cells_.clear();
    m_springs_.clear();

	// Create unique cells based on the preset - Stops duplicates
    std::unordered_set<int> unique_cells;
    for (const auto& connection : preset) 
    {
        unique_cells.insert(connection.first);
        unique_cells.insert(connection.second);
    }

	// Determine spawn area
	sf::Vector2f center;
	if (position == sf::Vector2f{ 0, 0 })
	{
        const float world_rad = m_world_bounds_->radius;
        center = Random::rand_pos_in_circle(m_world_bounds_->center, world_rad);
	}
    const Circle protozoa_area = { center, spawn_radius };

	// Create cells
    for (int cell_id : unique_cells) 
    {
        m_cells_.emplace_back(cell_id, protozoa_area.rand_pos());
    }

    // Create springs
    int i = 0;
    for (const auto& connection : preset) 
    {
        m_springs_.emplace_back(i++, connection.first, connection.second);
    }

	// Finally we need to tell the genome about these new cells and springs
    for (int cell_id : unique_cells)
    {
        add_cell_gene(cell_id);
    }
    for (int spring_id = 0; spring_id < m_springs_.size(); ++spring_id)
    {
        add_spring_gene(spring_id);
	}
}
