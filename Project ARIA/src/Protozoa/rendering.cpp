#include "Protozoa.h"
#include "../Utils/utility.h"
#include "../Utils/utility_SFML.h"



void Protozoa::render()
{
	render_cells();

	if (debug_mode_)
	{
		render_debug();
	}
}

void Protozoa::render_cells()
{
	for (Cell& cell : m_cells_)
	{
		render_cell_connections(cell, true);
	}

	for (Cell& cell : m_cells_)
	{
		const sf::Vector2f pos = cell.get_position();
		const float rad = cell.get_radius();

		// configuring the renderer to have the cell params.
		m_cell_renderer_ptr_->setPosition(pos - sf::Vector2f{ rad, rad });
		m_cell_renderer_ptr_->setRadius(rad);
		m_cell_renderer_ptr_->setFillColor(cell.cell_color_inner);
		m_cell_renderer_ptr_->setOutlineColor(cell.cell_color_outer);
		m_cell_renderer_ptr_->setOutlineThickness(CellSettings::cell_outline_thickness);

		m_window_ptr_->draw(*m_cell_renderer_ptr_);
	}
}



void Protozoa::render_debug()
{
	draw_rect_outline(m_personal_bounds_, *m_window_ptr_);

	for (Cell& cell : m_cells_)
	{
		const sf::Vector2f pos = cell.get_position();
		const float rad = cell.get_radius();

		// rendering the bounding boxes
		draw_rect_outline({ pos.x - rad, pos.y - rad, rad * 2, rad * 2 }, *m_window_ptr_);
		render_cell_connections(cell);

		// drawing the direction of the cell
		draw_direction(*m_window_ptr_, pos, cell.get_velocity(), rad * 2, 2, 4,
			{ 170, 200, 200 }, { 180, 225, 255 });
	}



	// protozoa information under the bounding box
	const sf::Vector2f start_pos = { m_personal_bounds_.left, m_personal_bounds_.top + m_personal_bounds_.height };

	const std::string combined_string = "cell count: " + std::to_string(m_cells_.size()) + "\n" +
		"frames old: " + std::to_string(frames_alive) + "\n" +
		"gen: " + std::to_string(generation) + "\n" +
		"energy: " + std::to_string(energy);

	m_info_font_.draw(start_pos, combined_string, false);
}



void Protozoa::render_cell_connections(Cell& cell, const bool thick_lines) const
{
	for (const Spring& spring : m_springs_)
	{
		const sf::Vector2f pos1 = m_cells_[spring.m_cellA_id].get_position();
		const sf::Vector2f pos2 = m_cells_[spring.m_cellB_id].get_position();

		if (thick_lines)
		{
			draw_thick_line(*m_window_ptr_, pos1, pos2, spring_thickness, spring_outline_thickness, spring.fill_color, spring.outline_color);
		}

		else
		{
			m_window_ptr_->draw(make_line(pos1, pos2, sf::Color::Magenta));
		}
	}
}