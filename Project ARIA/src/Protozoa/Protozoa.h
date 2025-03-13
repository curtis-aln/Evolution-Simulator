#pragma once
#include <SFML/Graphics.hpp>
#include "../settings.h"
#include "../Utils/Graphics/font_renderer.hpp"
#include "../Utils/utility.h"
#include "../Utils/utility_SFML.h"
#include "../Utils/Graphics/Circle.h"
#include "../Utils/Graphics/spatial_hash_grid.h"
#include "cell.h"
#include "spring.h"
#include "genetics.h"

#include "../Utils/o_vector.hpp"

// this is the organisms in the simulation, they are made up of cells which all act independently, attached by springs
// the protozoa class is responsible for connecting the springs and cells

inline static constexpr float initial_energy = 300.f; // energy the protozoa spawn with

class Protozoa : ProtozoaSettings
{
	sf::RenderWindow* m_window_ = nullptr; // for rendering // todo move all rendering outside this class
	Circle* m_world_bounds_ = nullptr;

	// components of the protozoa
	std::vector<Cell> m_cells_{};
	std::vector<Spring> m_springs_{};

	sf::Rect<float> m_personal_bounds_{};

	ProtozoaGenes m_genes_{}; // information which is passed from generation to generation
	
	float energy = initial_energy;
	unsigned frames_alive = 0u;
	unsigned generation = 0u;

	// When in debugging mode, you can pull at cells inside this protozoa | storing the location of it in memory 
	// -1 means none selected
	int selected_cell_id = -1;

	std::vector<Cell*> collision_vector;

public:
	using Protozoa_Vector = o_vector<Protozoa, WorldSettings::max_protozoa>;

	int id = 0;
	bool active = true; // for o_vector.h


	Protozoa(int id_ = 0, Circle* world_bounds = nullptr, sf::RenderWindow* window = nullptr, bool init_cells = false);

	void update();
	void render(bool debug_mode = false);

	// debugging and modifying settings
	bool is_hovered_on(sf::Vector2f mousePosition, bool tollarance_check = false) const;
	bool check_press(sf::Vector2f mouse_position);
	void deselect_cell();
	void make_connection(int cell1_id, int cell2_id);
	void builder_add_cell(sf::Vector2f center);
	void move_selected_cell(sf::Vector2f mouse_position);

	// information fetching
	Cell* get_selected_cell(sf::Vector2f mouse_pos);
	std::vector<Cell>& get_cells();
	sf::Rect<float> get_bounds() { return m_personal_bounds_; }

	// information setting
	void set_render_window(sf::RenderWindow* window);
	void set_bounds(Circle* bounds);

	sf::Vector2f get_center()
	{
		return m_personal_bounds_.getPosition() + m_personal_bounds_.getSize() / 2.f;
	}
	
	void bound_cells();

private:
	// rendering
	void render_debug();
	void render_cell_connections(Cell& cell, bool thick_lines = false) const;

	// updating
	void update_bounding_box();
	void update_springs();
	void update_cells();

	// initialisation
	void initialise_cells();
	void initialise_springs();
};
