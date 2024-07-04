#include "../Protozoa.h"

void Protozoa::render_debug()
{
	// This is the bounding box of the protozoa, used for external collision events
	draw_rect_outline(m_personal_bounds_, *m_window_ptr_);


	// for each cell we draw its bounding box
	for (Cell& cell : m_cells_)
	{
		const sf::Vector2f pos = cell.position;
		const float rad = cell.get_radius();

		// rendering the bounding boxes
		const sf::FloatRect rect = { pos.x - rad, pos.y - rad, rad * 2, rad * 2 };
		draw_rect_outline(rect, *m_window_ptr_);
		render_cell_connections(cell);

		// drawing the direction of the cell
		const float arrow_length = std::min(rad*4, length(cell.velocity) * rad);
		draw_direction(*m_window_ptr_, pos, cell.velocity, arrow_length, 6, 10,
			{ 200, 220, 200 }, { 190, 200, 190 });

		// drawing cell stats
		m_info_font_.draw({ rect.left, rect.top + rect.height + 5 }, "id: " + std::to_string(cell.rel_id), false);
	}

	// protozoa information under the bounding box
	const sf::Vector2f start_pos = { m_personal_bounds_.left, m_personal_bounds_.top + m_personal_bounds_.height + 10 };

	const std::string combined_string = "cell count: " + std::to_string(m_cells_.size()) + "\n" +
		"frames old: " + std::to_string(frames_alive) + "\n" +
		"gen: " + std::to_string(generation) + "\n" +
		"energy: " + std::to_string(energy);

	m_info_font_.draw(start_pos, combined_string, false);

	// spring information
	//for (const Spring& spring : m_springs_)
	//{
	//	const sf::Vector2f cell_A_pos = m_cells_[spring.m_cellA_id].get_position();
	//	const sf::Vector2f cell_B_pos = m_cells_[spring.m_cellB_id].get_position();
	//
	//	const sf::Vector2f mid_point = get_midpoint(cell_A_pos, cell_B_pos);
	//	const sf::Vector2f upper_quartile = get_midpoint(mid_point, cell_B_pos);
	//	const sf::Vector2f lower_quartile = get_midpoint(cell_A_pos, mid_point);
	//
	//	m_info_font_.draw(lower_quartile, vector_to_string(spring.direction_A_force, 2), true);
	//	m_info_font_.draw(upper_quartile, vector_to_string(spring.direction_B_force, 2), true);
	//}
}



void Protozoa::builder_add_cell(const sf::Vector2f center)
{
	const CellGenetics genetics = create_cell();

	Cell cell(genetics);

	const Circle circle(center, 200.f);
	cell.position = circle.rand_pos_in_circle();
	m_cells_.emplace_back(cell);
}


void Protozoa::move_selected_cell(const sf::Vector2f mouse_position)
{
	if (selected_cell_id >= 0)
	{
		m_cells_[selected_cell_id].position = mouse_position;
	}
}


void Protozoa::deselect_cell()
{
	if (selected_cell_id >= 0)
	{
		m_cells_[selected_cell_id].selected = false;
		selected_cell_id = -1;
	}
}


void Protozoa::make_connection(const int cell1_id, const int cell2_id)
{
	const SpringGenetics& genetics = create_cell_connection(cell1_id, cell2_id);
	m_springs_.emplace_back(genetics.connecting_cell_ids, genetics.colors, genetics.starting_rest_length,
		genetics.spring_constant, genetics.damping_constant);
}


bool Protozoa::is_hovered_on(const sf::Vector2f mousePosition) const
{
	return m_personal_bounds_.contains(mousePosition);
}

bool Protozoa::check_press(const sf::Vector2f mouse_position)
{
	Cell* selected_cell = get_selected_cell(mouse_position);

	if (selected_cell != nullptr)
	{
		selected_cell_id = selected_cell->rel_id;
		selected_cell->selected = true;
	}

	return selected_cell != nullptr;
}


Cell* Protozoa::get_selected_cell(const sf::Vector2f mouse_pos)
{
	if (is_hovered_on(mouse_pos))
	{
		for (Cell& cell : m_cells_)
		{
			const float dist_sq = dist_squared(cell.position, mouse_pos);
			const float rad = cell.get_radius();
			if (dist_sq < rad * rad)
			{
				return &cell;
			}
		}
	}
	return nullptr;
}

void Protozoa::set_debug_mode(const bool mode)
{
	debug_mode_ = mode;
}

std::vector<Cell>& Protozoa::get_cells()
{
	return m_cells_;
}


void Protozoa::set_render_window(sf::RenderWindow* window)
{
	m_window_ptr_ = window;
}

void Protozoa::set_bounds(Circle* bounds)
{
	m_world_bounds_ = bounds;
}

void Protozoa::set_renderer(sf::CircleShape* renderer)
{
	m_cell_renderer_ptr_ = renderer;
}