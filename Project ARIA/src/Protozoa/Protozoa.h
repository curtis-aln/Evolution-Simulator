#pragma once
#include <SFML/Graphics.hpp>
#include "../settings.h"
#include "../Utils/Graphics/font_renderer.hpp"
#include "../Utils/utility.h"
#include "../Utils/utility_SFML.h"
#include "../Utils/Graphics/Circle.h"
#include "../Utils/Graphics/spatial_hash_grid.h"
#include "../Utils/NeuralNetworks/GeneticNeuralNetwork.h"
#include "cell.h"
#include "spring.h"


#include "../Utils/o_vector.hpp"
#include "../food_manager.h"
#include "../Utils/random.h"


// this is the organisms in the simulation, they are made up of cells which all act independently, attached by springs
// the protozoa class is responsible for connecting the springs and cells

inline static constexpr float initial_energy = 300.f; // energy the protozoa spawn with

class Protozoa : ProtozoaSettings, GeneSettings
{
	sf::RenderWindow* m_window_ = nullptr; // for rendering // todo move all rendering outside this class
	Circle* m_world_bounds_ = nullptr;

	// components of the protozoa
	std::vector<Cell> m_cells_{};
	std::vector<Spring> m_springs_{};

	sf::Rect<float> m_personal_bounds_{};

	// information which is passed from generation to generation
	size_t cell_count = Random::rand_range(cell_amount_range);
	GeneticNeuralNetwork neural_network;

	// position and velocity tracking
	sf::Vector2f previous_position = {0, 0};
	sf::Vector2f velocity = { 0, 0 };

	
public:
	sf::Color cell_outer_color = Random::rand_color();
	sf::Color cell_inner_color = Random::rand_color();

	sf::Color spring_outer_color = Random::rand_color();
	sf::Color spring_inner_color = Random::rand_color();

	// debugging
	std::vector<sf::Vector2f> food_positions_nearby{};
	std::vector<sf::Vector2f> cell_positions_nearby{};

	float energy = initial_energy;
	unsigned frames_alive = 0u;
	unsigned generation = 0u;

	// When in debugging mode, you can pull at cells inside this protozoa | storing the location of it in memory 
	// -1 means none selected
	int selected_cell_id = -1;

	using Protozoa_Vector = o_vector<Protozoa, WorldSettings::max_protozoa>;

	int id = 0;
	bool active = true; // for o_vector.h

	bool reproduce = false;
	bool dead = false;

	Protozoa(int id_ = 0, Circle* world_bounds = nullptr, sf::RenderWindow* window = nullptr, bool init_cells = false);

	void update(FoodManager& food_manager);
	void handle_food(FoodManager& food_manager);
	void mutate();
	void add_cell();
	void remove_cell();
	void render_protozoa_springs();
	void render_debug(bool skeleton_mode);

	void draw_cell_outlines();

	void nearby_food_information();

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
	std::vector<Spring>& get_springs() { return m_springs_; }
	sf::Rect<float> get_bounds() { return m_personal_bounds_; }

	// information setting
	void set_render_window(sf::RenderWindow* window);
	void set_bounds(Circle* bounds);

	sf::Vector2f get_center()
	{
		return m_personal_bounds_.getPosition() + m_personal_bounds_.getSize() / 2.f;
	}
	
	void bound_cells();

	void set_cells_and_springs(std::vector<Cell>& cells, std::vector<Spring>& springs)
	{
		m_cells_ = cells;
		m_springs_ = springs;
	}

private:
	// rendering
	void draw_cell_physics();
	void draw_cell_stats_info();
	void draw_spring_information();
	void render_cell_connections(Cell& cell, bool thick_lines = false) const;

	// updating
	void update_bounding_box();
	void update_springs();
	void update_cells();

	// initialisation
	void initialise_cells();
	void initialise_springs();
};
