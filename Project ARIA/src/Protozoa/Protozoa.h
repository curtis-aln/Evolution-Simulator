#pragma once
#include <SFML/Graphics.hpp>
#include "../settings.h"
#include "../Utils/fonts/font_renderer.hpp"
#include "../Utils/utility.h"
#include "../Utils/utility_SFML.h"
#include "../Utils/Circle.h"
#include "cell.h"
#include "spring.h"

class Protozoa : ProtozoaSettings
{
	sf::RenderWindow* m_window_ptr_ = nullptr;
	sf::CircleShape* m_cell_renderer_ptr_ = nullptr;

	std::vector<Cell> m_cells_{};
	std::vector<Spring> m_springs_{};

	Circle* m_world_bounds_ = nullptr;
	sf::Rect<float> m_personal_bounds_{};

	bool debug_mode_ = false;
	Font m_info_font_;

	float energy = 100.f;
	unsigned frames_alive = 0u;
	unsigned generation = 0u;

	// debug
	int selected_cell_id = -1;


public:
	Protozoa(Circle* world_bounds = nullptr, sf::RenderWindow* window = nullptr, sf::CircleShape* cell_renderer = nullptr, bool init_cells = true);

	void update();
	void render();

	// debugging settings
	bool is_hovered_on(sf::Vector2f mousePosition) const;
	bool check_press(sf::Vector2f mouse_position);
	Cell* get_selected_cell(sf::Vector2f mouse_pos);
	void set_debug_mode(bool mode);
	std::vector<Cell>& get_cells();
	void deselect_cell();
	void make_connection(int cell1_id, int cell2_id);
	void builder_add_cell(const sf::Vector2f center);
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
	void create_children_for_cell(Cell& cell, float probability, int depth, bool is_parent);
	void create_cell(Cell* parent, float probability, int depth);
	bool does_spring_exist_between(int cellA_id, int cellB_id) const;
	static sf::Vector2f create_cell_position(sf::Vector2f relative_center, float spawn_range);
	static void create_cellular_connection(Cell* parent_cell, Cell* child_cell);
};
