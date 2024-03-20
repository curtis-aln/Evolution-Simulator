#pragma once
#include <SFML/Graphics.hpp>
#include "../settings.h"


class Cell : CellSettings
{
	Cell* m_parent = nullptr;
	std::vector<int> children_ids{};

	sf::Vector2f m_position{};
	sf::Vector2f m_velocity{};

	float m_radius = 10.f;
	float m_direction{};

	sf::Color m_cell_color_inner = { 200, 215, 161 };
	sf::Color m_cell_color_outer = { 140, 154, 110 };


public:
	int _relID;

	Cell(Cell* parent = nullptr, int relId = 0) : m_parent(parent), _relID(relId)
	{
		
	}

	void update()
	{
		
	}

	void render(sf::RenderWindow* window, sf::CircleShape* renderer)
	{
		// configuring the renderer to have the cell paramas
		renderer->setPosition(m_position - sf::Vector2f{ m_radius, m_radius });
		renderer->setRadius(m_radius);
		renderer->setFillColor(m_cell_color_inner);
		renderer->setOutlineColor(m_cell_color_outer);
		renderer->setOutlineThickness(m_radius / 6);

		window->draw(*renderer);
	}

	void render_debug(sf::RenderWindow* window)
	{
		
	}

	void set_position(const sf::Vector2f new_position)
	{
		m_position = new_position;
	}

	sf::Vector2f get_position() const { return m_position; }
	float get_radius() const { return m_radius; }

	// relationship management
	void set_parent(Cell* parent) { m_parent = parent; }
	bool is_parent(const Cell* query_cell) const { return query_cell == m_parent; }
	bool is_child(const int query_cell_id) const
	{
		for (int id : children_ids)
		{
			if (id == query_cell_id)
				return true;
		}
		return false;
	}
	void add_child(const int child_id)
	{
		children_ids.push_back(child_id);
	}
};



class Protozoa : ProtozoaSettings
{
	sf::RenderWindow* m_window = nullptr;
	sf::CircleShape* m_cell_renderer = nullptr;

	std::vector<Cell> m_cells;

	sf::Rect<float> m_world_bounds{};
	sf::Rect<float> m_personal_bounds{};

	bool debug_mode = false;


public:
	Protozoa(const sf::Rect<float>& bounds = sf::Rect<float>(), sf::RenderWindow* window = nullptr, sf::CircleShape* cell_renderer = nullptr);

	void update();
	bool is_hovered_on(sf::Vector2f mousePosition) const;
	void set_debug_mode(bool mode);
	void render();

private:
	void render_debug();
	void update_bounds();
	void initialise_cells();
	void set_parent_cell(Cell* cell);
	static bool create_cellular_connection(Cell* parent_cell, Cell* child_cell);
};
