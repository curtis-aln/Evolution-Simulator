#include "Protozoa.h"

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



void Protozoa::builder_add_cell(const sf::Vector2f center)
{
	Cell cell{ nullptr, static_cast<int>(m_cells_.size()) };

	cell.set_position(create_cell_position(center, 50));
	m_cells_.emplace_back(cell);
}