#include "../Protozoa.h"
#include "../../Utils/utility.h"
#include "../../Utils/utility_SFML.h"

// Render file handles all the rendering for the protozoa (Protozoa.h)


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
		const sf::Vector2f pos = cell.position_;
		const float rad = cell.get_radius();

		// configuring the renderer to have the cell params.
		m_cell_renderer_ptr_->setPosition(pos - sf::Vector2f{ rad, rad });
		m_cell_renderer_ptr_->setRadius(rad);
		m_cell_renderer_ptr_->setFillColor(cell.color_);
		m_cell_renderer_ptr_->setOutlineColor(cell.outline_color_);
		m_cell_renderer_ptr_->setOutlineThickness(CellSettings::cell_outline_thickness);

		m_window_ptr_->draw(*m_cell_renderer_ptr_);
	}
}


void Protozoa::render_cell_connections(Cell& cell, const bool thick_lines) const
{
	for (const Spring& spring : m_springs_)
	{
		const sf::Vector2f pos1 = m_cells_[spring.connection.first].position_;
		const sf::Vector2f pos2 = m_cells_[spring.connection.second].position_;

		if (thick_lines)
		{
			draw_thick_line(*m_window_ptr_, pos1, pos2, spring_thickness, 
				spring_outline_thickness, spring.coloring.first, spring.coloring.second);
		}

		else
		{
			m_window_ptr_->draw(make_line(pos1, pos2, sf::Color::Magenta));
		}
	}
}