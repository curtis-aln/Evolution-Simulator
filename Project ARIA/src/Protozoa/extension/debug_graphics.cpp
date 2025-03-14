#include "../Protozoa.h"
#include "../../settings.h"

inline static constexpr float bounds_tollarance = CellSettings::cell_radius * 3.f;

void Protozoa::render(bool debug_mode)
{
	for (Cell& cell : m_cells_)
	{
		render_cell_connections(cell, true);
	}

	if (debug_mode)
	{
		render_debug();
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

void Protozoa::render_debug()
{
	// This is the bounding box of the protozoa, used for external collision events
	draw_rect_outline(m_personal_bounds_, *m_window_);

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
		const float arrow_length = std::min(rad*4, length(cell.velocity_) * rad);
		draw_direction(*m_window_, pos, cell.velocity_, arrow_length, 6, 10,
			{ 200, 220, 200 }, { 190, 200, 190 });

		// drawing cell stats
		font.draw({ rect.left, rect.top + rect.height + 5 }, "id: " + std::to_string(cell.id), false);
	}

	// protozoa information under the bounding box
	const sf::Vector2f start_pos = { m_personal_bounds_.left, m_personal_bounds_.top + m_personal_bounds_.height + 10 };

	const std::string combined_string = "cell count: " + std::to_string(m_cells_.size()) + "\n" +
		"frames old: " + std::to_string(frames_alive) + "\n" +
		"gen: " + std::to_string(generation) + "\n" +
		"energy: " + std::to_string(energy);

	font.draw(start_pos, combined_string, false);

	// spring information
	for (const Spring& spring : m_springs_)
	{
		const sf::Vector2f cell_A_pos = m_cells_[spring.cell_A_id].position_;
		const sf::Vector2f cell_B_pos = m_cells_[spring.cell_B_id].position_;
	
		const sf::Vector2f mid_point = get_midpoint(cell_A_pos, cell_B_pos);
		const sf::Vector2f upper_quartile = get_midpoint(mid_point, cell_B_pos);
		const sf::Vector2f lower_quartile = get_midpoint(cell_A_pos, mid_point);
	
		TextSettings::cell_statistic_font.draw(lower_quartile, vector_to_string(spring.direction_A_force, 2), true);
		TextSettings::cell_statistic_font.draw(upper_quartile, vector_to_string(spring.direction_B_force, 2), true);
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
