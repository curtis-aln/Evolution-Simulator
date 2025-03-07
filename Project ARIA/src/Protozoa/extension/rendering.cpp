#include "../Protozoa.h"
#include "../../Utils/utility.h"
#include "../../Utils/utility_SFML.h"

// Render file handles all the rendering for the protozoa (Protozoa.h)


void Protozoa::render()
{
	for (Cell& cell : m_cells_)
	{
		render_cell_connections(cell, true);
	}

	if (debug_mode_)
	{
		render_debug();
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