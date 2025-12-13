#pragma once
#include <SFML/Graphics.hpp>
#include "../settings.h"
#include "../Utils/Graphics/Circle.h"
#include "cell.h"
#include "spring.h"


#include "../Utils/o_vector.hpp"
#include "../food_manager.h"
#include "genome.h"

// this is the organisms in the simulation, they are made up of cells which all act independently, attached by springs
// the protozoa class is responsible for connecting the springs and cells

inline static constexpr float initial_energy = 300.f; // energy the protozoa spawn with

// The Genome class handles anything that has to do with mutation and genetics
class Protozoa : ProtozoaSettings, public Genome
{
	sf::RenderWindow* m_window_ = nullptr;
	Circle* m_world_bounds_ = nullptr;

	// components of the protozoa
	std::vector<Cell> m_cells_{};
	std::vector<Spring> m_springs_{};

	sf::Rect<float> m_personal_bounds_{};

	// position and velocity tracking
	sf::Vector2f previous_position = {0, 0};
	sf::Vector2f velocity = { 0, 0 };

	// reproduction related parameters
	unsigned stomach = 0;
	unsigned total_food_eaten = 0;
	
public:
	// debugging
	std::vector<sf::Vector2f> food_positions_nearby{};
	std::vector<sf::Vector2f> cell_positions_nearby{};

	float energy = initial_energy;
	unsigned frames_alive = 0u;

	// When in debugging mode, you can pull at cells inside this protozoa | storing the location of it in memory 
	// -1 means none selected
	int selected_cell_id = -1;

	using Protozoa_Vector = o_vector<Protozoa, WorldSettings::max_protozoa>;

	int id = 0;
	bool active = true; // for o_vector.h

	int offspring_count = 0;
	bool reproduce = false;
	bool dead = false;

	Protozoa(int id_ = 0, Circle* world_bounds = nullptr, sf::RenderWindow* window = nullptr, bool init_cells = false);

	void update(FoodManager& food_manager, bool debug);
	void handle_food(FoodManager& food_manager, bool debug);
	void mutate();
	void add_cell();
	static void remove_cell();
	void load_preset(Preset& preset, sf::Vector2f position = {0, 0});
	void render_protozoa_springs();
	void render_debug(bool skeleton_mode, bool show_connections, bool show_bounding_boxes);

	void draw_cell_outlines() const;

	void nearby_food_information() const;

	// debugging and modifying settings
	bool is_hovered_on(sf::Vector2f mousePosition, bool tollarance_check = false) const;
	bool check_press(sf::Vector2f mouse_position);
	void deselect_cell();
	static void make_connection(int cell1_id, int cell2_id);
	static void builder_add_cell(sf::Vector2f center);
	void move_selected_cell(sf::Vector2f mouse_position);

	// information fetching
	Cell* get_selected_cell(sf::Vector2f mouse_pos);
	std::vector<Cell>& get_cells();
	std::vector<Spring>& get_springs() { return m_springs_; }
	sf::Rect<float> get_bounds() const { return m_personal_bounds_; }

	// information setting
	void set_render_window(sf::RenderWindow* window);
	void set_bounds(Circle* bounds);

	sf::Vector2f get_center() const
	{
		return m_personal_bounds_.getPosition() + m_personal_bounds_.getSize() / 2.f;
	}
	
	void bound_cells();

	void reset_protozoa()
	{
		frames_alive = 0.f;
		dead = false;

		stomach = 0;
		total_food_eaten = 0;
		offspring_count = 0;
		energy = initial_energy;

		for (Cell& cell : m_cells_)
		{
			cell.reset();
		}
	}


	void set_protozoa_attributes(Protozoa* other)
	{
		m_cells_ = other->m_cells_;
		m_springs_ = other->m_springs_;
		
		reset(*other); // genome

		int idx = 0;
		for (Cell& cell : m_cells_)
		{
			cell.generation = other->m_cells_[idx++].generation;
		}
	}

	void generate_random();


private:
	// rendering
	void draw_cell_physics() const;
	void draw_protozoa_information();
	void draw_spring_information() const;
	void render_cell_connections(Cell& cell, bool thick_lines = false) const;

	// updating
	void update_bounding_box();
	void update_springs();
	void update_cells();
};
