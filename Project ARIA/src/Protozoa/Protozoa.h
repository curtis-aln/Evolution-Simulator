#pragma once
#include <SFML/Graphics.hpp>
#include "../settings.h"
#include "../Utils/font_renderer.hpp"
#include "../Utils/utility.h"
#include "../Utils/utility_SFML.h"
#include "../Utils/Circle.h"
#include "cell.h"
#include "spring.h"
#include "genetics.h"

class Protozoa : ProtozoaSettings
{
	sf::RenderWindow* m_window_ptr_ = nullptr;
	sf::CircleShape* m_cell_renderer_ptr_ = nullptr;

	std::vector<Cell> m_cells_{};
	std::vector<Spring> m_springs_{};

	Circle* m_world_bounds_ = nullptr;
	sf::Rect<float> m_personal_bounds_{};

	ProtozoaGenes m_genes_{};
	
	float energy = 100.f;
	unsigned frames_alive = 0u;
	unsigned generation = 0u;

	// debug
	int selected_cell_id = -1;

	bool debug_mode_ = false;

public:
	Protozoa(Circle* world_bounds = nullptr, sf::RenderWindow* window = nullptr, sf::CircleShape* cell_renderer = nullptr, 
		bool init_cells = false);

	void update();
	void render();

	// debugging settings
	bool is_hovered_on(sf::Vector2f mousePosition) const;
	bool check_press(sf::Vector2f mouse_position);
	Cell* get_selected_cell(sf::Vector2f mouse_pos);
	void set_debug_mode(bool mode);
	std::vector<Cell>& get_cells();
	void set_render_window(sf::RenderWindow* window);
	void set_bounds(Circle* bounds);
	void set_renderer(sf::CircleShape* renderer);
	void deselect_cell();
	void make_connection(int cell1_id, int cell2_id);
	
	void builder_add_cell(sf::Vector2f center);
	void move_selected_cell(sf::Vector2f mouse_position);

private:
	// debugging settings
	void render_debug();

	// rendering
	void render_cells();
	void render_cell_connections(Cell& cell, bool thick_lines = false) const;

	// updating
	void update_bounds();
	void update_springs();
	void update_cells();

	
	// initialisation
	void initialise_cells();
	void initialise_springs();
};
