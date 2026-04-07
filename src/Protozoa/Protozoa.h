#pragma once
#include <SFML/Graphics.hpp>
#include "../settings.h"
#include "../Utils/Graphics/Circle.h"
#include "cell.h"
#include "spring.h"


#include "../Utils/o_vector.hpp"
#include "../Food/food_manager.h"
#include "genetics/GenomeManager.h"

// this is the organisms in the simulation, they are made up of cells which all act independently, attached by springs
// the protozoa class is responsible for connecting the springs and cells

inline static constexpr float initial_energy = 300.f; // energy the protozoa spawn with



// The Genome class handles anything that has to do with mutation and genetics
class Protozoa : ProtozoaSettings, public GenomeManager
{
public:
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

	size_t time_since_last_reproduced = 0;
	
	// debugging
	sf::Vector2f birth_location = { 0, 0 };

	std::vector<sf::Vector2f> food_positions_nearby{};
	std::vector<sf::Vector2f> cell_positions_nearby{};

	float energy = initial_energy;
	unsigned frames_alive = 0u;

	float energy_lost_to_springs = 0.f;

	int id = 0;
	bool active = true; // for o_vector.h

	int offspring_count = 0;
	bool reproduce = false;
	bool dead = false;

	FixedSpan<obj_idx> nearby_food_container{ FoodSettings::cell_max_capacity * 9 };

	Protozoa(int id_ = 0, Circle* world_bounds = nullptr, sf::RenderWindow* window = nullptr);

	void update(FoodManager& food_manager, bool debug, float min_speed);
	void handle_food(FoodManager& food_manager, bool debug);
	void record_nearby_food(Food* food);
	void consume(Food* food, FoodManager& food_manager);

	void mutate(const bool artificial_add_cell = false, const float artificial_mutatation_rate = 0.f, const float artificial_mutatation_range = 0.f);
	void add_cell();
	void remove_spring();
	void render_protozoa_springs();
	void render_debug(Font* font, bool skeleton_mode, bool show_connections, bool show_bounding_boxes);

	void draw_cell_outlines();


	void reproduce_check();

	void nearby_food_information() const;
	void update_generation();

	// debugging and modifying settings
	bool is_hovered_on(sf::Vector2f mousePosition, bool tollarance_check = false) const;
	bool check_press(sf::Vector2f mouse_position);
	static void make_connection(int cell1_id, int cell2_id);
	void move_selected_cell(sf::Vector2f mouse_position);

	// information fetching
	Cell* get_selected_cell(sf::Vector2f mouse_pos);
	std::vector<Cell>& get_cells();
	std::vector<Spring>& get_springs() { return m_springs_; }
	sf::Rect<float> get_bounds() const { return m_personal_bounds_; }

	// information setting
	void set_render_window(sf::RenderWindow* window);
	void set_bounds(Circle* bounds);
	void move_center_location_to(const sf::Vector2f new_center)
	{
		const sf::Vector2f center = get_center();
		const sf::Vector2f translation = new_center - center;
		for (Cell& cell : m_cells_)
		{
			cell.position_ += translation;
		}
		update_bounding_box();
	}

	sf::Vector2f get_center() const
	{
		return m_personal_bounds_.position + m_personal_bounds_.size / 2.f;
	}
	
	void bound_cells();

	void soft_reset()
	{
		frames_alive = 0.f;
		dead = false;
		reproduce = false;

		stomach = 0;
		total_food_eaten = 0;
		offspring_count = 0;
		energy = initial_energy;

		time_since_last_reproduced = 0;

		for (Cell& cell : m_cells_)
		{
			cell.reset();
		}
	}


	void hard_reset()
	{
		soft_reset();
		
		// setting the containers back to zero size, for when we are restarting the simulation
		m_cells_.clear();
		m_springs_.clear();
		
		m_personal_bounds_ = { {0.f, 0.f}, {0.f, 0.f } };

		// position and velocity tracking
		previous_position = { 0, 0 };
		velocity = { 0, 0 };
		birth_location = { 0, 0 };


		food_positions_nearby.clear();
		cell_positions_nearby.clear();

		active = true; // for o_vector.h
	}

	void set_protozoa_attributes(Protozoa* other)
	{
		m_cells_ = other->m_cells_;
		m_springs_ = other->m_springs_;

		update_bounding_box();
	}

private:
	// rendering
	void draw_cell_physics(Font* font);
	void draw_spring_information(Font* font) const;
	void render_cell_connections(Cell& cell, bool thick_lines = false) const;

	// updating
	void update_bounding_box();
	void update_springs();
	void update_cells();

	void mutate_existing_cells(float mut_rate = 0.f, float mut_range = 0.f);
	void mutate_existing_springs(float mut_rate = 0.f, float mut_range = 0.f);
	bool cell_wander_check(Cell& cell);
	
	void remove_cell();
	void add_spring();
	void check_death_conditions(float min_speed);

};
