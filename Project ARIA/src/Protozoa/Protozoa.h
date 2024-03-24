#pragma once
#include <SFML/Graphics.hpp>
#include "../settings.h"
#include "../Utils/font_renderer.hpp"
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


public:
	Protozoa(Circle* world_bounds = nullptr, sf::RenderWindow* window = nullptr, sf::CircleShape* cell_renderer = nullptr);

	void update();
	void update_springs();
	void update_cells();
	bool is_hovered_on(sf::Vector2f mousePosition) const;
	void set_debug_mode(bool mode);
	void render();
	void render_cells();

private:
	void render_debug();
	void render_cell_connections(Cell& cell, const bool thick_lines = false) const;
	void update_bounds();
	void initialise_cells();
	void create_children_for_cell(Cell& cell, float probability, int depth, bool is_parent);
	void create_cell(Cell* parent, float probability, int depth);
	void initialise_springs();
	bool does_spring_exist_between(int cellA_id, int cellB_id) const;
	static void create_cellular_connection(Cell* parent_cell, Cell* child_cell);
};
