#include "../Protozoa.h"
#include "../../food_manager.h"

#include <vector>

#include <unordered_set>

inline float soften_clamp(const float value, const float minv, const float maxv, const float softness = 0.15f)
{
    // Pulls values gently back into range without killing exploration
    if (value < minv)
        return minv + (value - minv) * softness;
    if (value > maxv)
        return maxv + (value - maxv) * softness;
    return value;
}

inline sf::Color mutate_color(const sf::Color& color, const float mutation_rate = 0.002f)
{
    // Convert RGB -> [0,1]
    float r = color.r / 255.f;
    float g = color.g / 255.f;
    float b = color.b / 255.f;

    float maxv = std::max({ r, g, b });
    float minv = std::min({ r, g, b });
    float delta = maxv - minv;

    // --- RGB -> HSL ---
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

    // --- Mutation ---
    h += Random::rand_range(-20.f, 20.f);
    s += Random::rand_range(-mutation_rate, mutation_rate);
    l += Random::rand_range(-mutation_rate, mutation_rate);

    // Wrap hue to [0,360)
    h = fmod(h + 360.f, 360.f);

    // --- Organic biological palette constraints ---

    // Hue: avoid harsh red & neon colours, but still allow full range softly
    // (keeps blue–green–yellow–purple cluster typical in microbiology)
    h = soften_clamp(h, 20.f, 320.f, 0.25f);

    // Saturation: organic, non-neon
    s = soften_clamp(s, 0.25f, 0.75f, 0.20f);

    // Lightness: translucent soft look
    l = soften_clamp(l, 0.35f, 0.70f, 0.20f);

    // Clamp hard to prevent overflow
    s = std::clamp(s, 0.f, 1.f);
    l = std::clamp(l, 0.f, 1.f);

    // --- HSL -> RGB ---
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
        static_cast<sf::Uint8>((r1 + m) * 255),
        static_cast<sf::Uint8>((g1 + m) * 255),
        static_cast<sf::Uint8>((b1 + m) * 255)
    );
}


void Protozoa::handle_food(FoodManager& food_manager, const bool debug)
{
	const sf::Vector2f center = get_center();
	c_Vec<food_manager.spatial_hash_grid.max_nearby_capacity>& nearby =
		food_manager.spatial_hash_grid.find(center);

    if (debug)
    {
        food_positions_nearby.clear();
    }

	for (int i = 0; i < nearby.size; ++i)
	{
		Food* food_particle = food_manager.at(nearby.at(i));

        if (debug)
        {
            food_positions_nearby.push_back(food_particle->position);
        }

		if (m_personal_bounds_.contains(food_particle->position)) // todo better collision handling
		{
			food_manager.remove_food(food_particle->id);

            total_food_eaten++;
            stomach++;
            energy += 10;

            if (stomach > 1)
            {
                stomach = 0;
                reproduce = true;
                offspring_count++;
            }
		}
	}
}


void Protozoa::mutate()
{
	// mutations: mutate genome and get flags for adding/removing cells
    std::pair<bool, bool> mutations = mutate_genome();

    // adding new components to the organism
    if (mutations.first)
    {
        add_cell();
    }

    if (mutations.second)
    {
        remove_cell();
    }
}

void Protozoa::add_cell()
{
    // finding the parent which will undergo mitosis
    const int parent_index = Random::rand_range(size_t(0), m_cells_.size() - 1);
    sf::Vector2f position = Random::rand_pos_in_circle(m_cells_[parent_index].position_, m_cells_[parent_index].radius);

    // creating the new cell and adding it to our cells
    const int cell_id = m_cells_.size();
    Cell new_cell{ cell_id, id, position};
    m_cells_.push_back(new_cell);

    // creating a spring connection to that cell
	const auto id = static_cast<int>(m_springs_.size());
    Spring new_spring{ id, parent_index, cell_id };
    m_springs_.push_back(new_spring);

	// Now we need to update our genome dictionaries to account for the new cell and spring
    add_cell_gene(cell_id);
	add_spring_gene(cell_id);

	// Finally we tell the cell and spring about their genes
	m_cells_.back().set_cell_gene(get_cell_gene(cell_id));
    m_springs_.back().set_spring_gene(get_spring_gene(cell_id));

}

void Protozoa::remove_cell()
{
    // todo
}


void Protozoa::load_preset(Preset& preset)
{
    // Clear existing genetic data
    m_cells_.clear();
    m_springs_.clear();

    // Create unique cells based on the preset
    std::unordered_set<int> unique_cells;
    for (const auto& connection : preset) 
    {
        unique_cells.insert(connection.first);
        unique_cells.insert(connection.second);
    }

    // Initialize cells
    const float world_rad = m_world_bounds_->radius;
    sf::Vector2f center = Random::rand_pos_in_circle(m_world_bounds_->center, world_rad);
    const Circle protozoa_area = { center, spawn_radius };

    for (int cell_id : unique_cells) 
    {
        m_cells_.emplace_back(cell_id, id, protozoa_area.rand_pos());
    }

    // Create springs
    int i = 0;
    for (const auto& connection : preset) 
    {
        m_springs_.emplace_back(i++, connection.first, connection.second);
    }

    init_cell_genome_dictionaries();
	init_spring_genome_dictionaries();

    update_spring_gene_connections();
    update_cell_gene_connections();
}


// Initialize gene dictionaries from existing cells and springs.
    // For any cell/spring that lacks a gene entry, a default gene will be created.
void Protozoa::init_cell_genome_dictionaries()
{
    // Ensure cell genes exist for each cell id
    for (const Cell& c : m_cells_)
    {
        int id = static_cast<int>(c.id);
        if (cell_genes.find(id) == cell_genes.end())
        {
            cell_genes.emplace(id, CellGene());
        }
    }

    // Remove orphaned cell genes
    for (auto it = cell_genes.begin(); it != cell_genes.end(); )
    {
        int id = it->first;
        auto found = std::find_if(m_cells_.begin(), m_cells_.end(), [id](const Cell& c) { return static_cast<int>(c.id) == id; });
        if (found == m_cells_.end()) it = cell_genes.erase(it);
        else ++it;
    }
}

void Protozoa::init_spring_genome_dictionaries()
{
    // Ensure spring genes exist for each spring id
    for (const Spring& s : m_springs_)
    {
        int id = static_cast<int>(s.id);
        if (spring_genes.find(id) == spring_genes.end())
        {
            spring_genes.emplace(id, SpringGene());
        }
    }

    // Remove orphaned spring genes
    for (auto it = spring_genes.begin(); it != spring_genes.end(); )
    {
        int id = it->first;
        auto found = std::find_if(m_springs_.begin(), m_springs_.end(), [id](const Spring& s) { return static_cast<int>(s.id) == id; });
        if (found == m_springs_.end()) it = spring_genes.erase(it);
        else ++it;
    }
}

void Protozoa::update_spring_gene_connections()
{
    // Tell each spring about its gene
    for (Spring& spring : m_springs_)
    {
        spring.set_spring_gene(get_spring_gene(static_cast<int>(spring.id)));
    }
}

// Now for cells
void Protozoa::update_cell_gene_connections()
{
    for (Cell& cell : m_cells_)
    {
        cell.set_cell_gene(get_cell_gene(static_cast<int>(cell.id)));
    }
}