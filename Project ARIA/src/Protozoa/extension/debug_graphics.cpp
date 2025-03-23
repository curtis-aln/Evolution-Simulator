#include "../Protozoa.h"
#include "../../settings.h"
#include "../../Utils/utility.h"

inline static constexpr float bounds_tollarance = CellSettings::cell_radius * 3.f;

void Protozoa::render_protozoa_springs()
{
	for (Cell& cell : m_cells_)
	{
		render_cell_connections(cell, true);
	}
}

void Protozoa::render_debug(bool skeleton_mode)
{
	if (skeleton_mode)
	{
		draw_cell_outlines();
	}


	// This is the bounding box of the protozoa, used for external collision events
	draw_rect_outline(m_personal_bounds_, *m_window_);

	draw_cell_physics();
	draw_cell_stats_info();
	draw_spring_information();
	nearby_food_information();
}

void Protozoa::draw_cell_outlines()
{
	sf::CircleShape circle_outline;
	
	for (Cell& cell : m_cells_)
	{
		const sf::Vector2f pos = cell.position_;
		const float rad = cell.radius + CellSettings::cell_outline_thickness;

		circle_outline.setRadius(rad);
		circle_outline.setFillColor({ 0, 0, 0 });
		circle_outline.setPosition(pos - sf::Vector2f{rad, rad});
		m_window_->draw(circle_outline);

		circle_outline.setFillColor({ 255, 0, 255 });
		circle_outline.setRadius(rad/3);
		circle_outline.setPosition(pos - sf::Vector2f{ rad/3, rad/3 });
		m_window_->draw(circle_outline);
	}


}

void Protozoa::nearby_food_information()
{
	const sf::Vector2f center = get_center();
	for (sf::Vector2f pos : food_positions_nearby)
	{
		m_window_->draw(make_line(center, pos, sf::Color::Magenta));
	}

	for (sf::Vector2f pos : cell_positions_nearby)
	{
		m_window_->draw(make_line(center, pos, sf::Color::Yellow));
	}
}


void Protozoa::render_cell_connections(Cell& cell, const bool thick_lines) const
{
	for (const Spring& spring : m_springs_)
	{
		const sf::Vector2f pos1 = m_cells_[spring.cell_A_id].position_;
		const sf::Vector2f pos2 = m_cells_[spring.cell_B_id].position_;

		if (thick_lines)
		{
			draw_thick_line(*m_window_, pos1, pos2, spring_thickness,
				spring_outline_thickness, spring.outer_color, spring.inner_color);
		}

		else
		{
			m_window_->draw(make_line(pos1, pos2, sf::Color::Magenta));
		}
	}
}


void Protozoa::draw_cell_physics()
{
	Font& font = TextSettings::cell_statistic_font;

	// for each cell we draw its bounding box
	for (Cell& cell : m_cells_)
	{
		const sf::Vector2f pos = cell.position_;
		const float rad = cell.radius;

		// rendering the bounding boxes
		const sf::FloatRect rect = { pos.x - rad, pos.y - rad, rad * 2, rad * 2 };
		draw_rect_outline(rect, *m_window_);
		render_cell_connections(cell);

		// drawing the direction of the cell
		const float arrow_length = std::min(rad * 4, length(cell.velocity_) * rad);
		draw_direction(*m_window_, pos, cell.velocity_, arrow_length, 6, 10,
			{ 200, 220, 200 }, { 190, 200, 190 });

		// drawing cell stats
		font.draw(pos, "id: " + std::to_string(cell.id), false);
	}
}


void Protozoa::draw_cell_stats_info()
{
	Font& font = TextSettings::cell_statistic_font;

	// protozoa information under the bounding box
	sf::Vector2f start_pos = { m_personal_bounds_.left, m_personal_bounds_.top + m_personal_bounds_.height + 10 };

	const std::string combined_string =
		"id: " + std::to_string(id) +"\n" +
		"cells: " + std::to_string(m_cells_.size()) + " springs: " + std::to_string(m_springs_.size()) + "\n" +
		"age: " + std::to_string(frames_alive) + "\n" +
		"generation: " + std::to_string(generation) + "\n" +
		"energy: " + denary_to_str(energy, 1);

	font.draw(start_pos, combined_string, false);

	// top statistics
	const float speed = length(velocity);

	start_pos = { m_personal_bounds_.left, m_personal_bounds_.top - 70 };
	const std::string text = "position: " + vector_to_string(get_center(), 1)
		+ "\nvelocity: " + vector_to_string(velocity, 1) + "\nspeed: " + denary_to_str(speed, 1);
	font.draw(start_pos, text, false);
}


void Protozoa::draw_spring_information()
{
	// spring information
	for (const Spring& spring : m_springs_)
	{
		const sf::Vector2f cell_A_pos = m_cells_[spring.cell_A_id].position_;
		const sf::Vector2f cell_B_pos = m_cells_[spring.cell_B_id].position_;

		const sf::Vector2f mid_point = get_midpoint(cell_A_pos, cell_B_pos);
		const sf::Vector2f upper_quartile = get_midpoint(mid_point, cell_B_pos);
		const sf::Vector2f lower_quartile = get_midpoint(cell_A_pos, mid_point);

		sf::Vector2f f1 = spring.direction_A_force;
		sf::Vector2f f2 = spring.direction_B_force;

		std::ostringstream vectorF1;
		vectorF1 << "(" << denary_to_str(f1.x, 1) << ", " << denary_to_str(f1.y, 1) << ")";
		std::ostringstream vectorF2;
		vectorF2 << "(" << denary_to_str(f2.x, 1) << ", " << denary_to_str(f2.y, 1) << ")";
		
		TextSettings::cell_statistic_font.draw(lower_quartile, vectorF1.str(), true);
		TextSettings::cell_statistic_font.draw(upper_quartile, vectorF2.str(), true);
	}
}


void Protozoa::builder_add_cell(const sf::Vector2f center)
{
	// todo
	//const CellGene genetics = create_cell();

	//Cell cell(genetics);

	//const Circle circle(center, 200.f);
	//cell.position_ = circle.rand_pos();
	//m_cells_.emplace_back(cell);
}


void Protozoa::move_selected_cell(const sf::Vector2f mouse_position)
{
	if (selected_cell_id >= 0)
	{
		m_cells_[selected_cell_id].position_ = mouse_position;
	}
}


void Protozoa::deselect_cell()
{
	if (selected_cell_id >= 0)
	{
		selected_cell_id = -1;
	}
}


void Protozoa::make_connection(const int cell1_id, const int cell2_id)
{
	// todo
	//const SpringGene& genetics = create_cell_connection(cell1_id, cell2_id);
	//m_springs_.emplace_back(genetics.connecting_cell_ids, genetics.colors, genetics.rest_length,
	//	genetics.spring_constant, genetics.damping);
}


bool Protozoa::is_hovered_on(const sf::Vector2f mousePosition, bool tollarance_check) const
{
	if (tollarance_check)
	{
		sf::FloatRect resized_rect = resize_rect(m_personal_bounds_, { -bounds_tollarance , -bounds_tollarance });
		return resized_rect.contains(mousePosition);
	}
	return m_personal_bounds_.contains(mousePosition);
}

bool Protozoa::check_press(const sf::Vector2f mouse_position)
{
	Cell* selected_cell = get_selected_cell(mouse_position);

	if (selected_cell != nullptr)
	{
		selected_cell_id = selected_cell->id;
	}

	if (m_personal_bounds_.contains(mouse_position))
	{
		return true;
	}

	return selected_cell != nullptr;
}


Cell* Protozoa::get_selected_cell(const sf::Vector2f mouse_pos)
{
	if (is_hovered_on(mouse_pos))
	{
		for (Cell& cell : m_cells_)
		{
			const float dist_sq = dist_squared(cell.position_, mouse_pos);
			const float rad = cell.radius;
			if (dist_sq < rad * rad)
			{
				return &cell;
			}
		}
	}
	return nullptr;
}

std::vector<Cell>& Protozoa::get_cells()
{
	return m_cells_;
}


void Protozoa::set_render_window(sf::RenderWindow* window)
{
	m_window_ = window;
}

void Protozoa::set_bounds(Circle* bounds)
{
	m_world_bounds_ = bounds;
}
