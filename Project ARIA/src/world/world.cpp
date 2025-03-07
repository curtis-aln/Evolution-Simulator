#include "world.h"
#include "../Utils/utility_SFML.h"
#include "../Utils/buffer_renderer.h"

World::World(sf::RenderWindow* window)
	: m_window_(window),
	border_render_(make_circle(m_bounds_.radius, m_bounds_.center))
{
	init_organisms();
	init_food();
	init_environment();

	const size_t protozoa_count = m_all_protozoa_.size();
	const size_t predicted_cells = protozoa_count * ProtozoaSettings::max_cells;

	// reserving nessesary data
	outer_color_data.reserve(predicted_cells);
	inner_color_data.reserve(predicted_cells);
	position_data.reserve(predicted_cells);

}

void World::update_world()
{
	for (Protozoa& protozoa : m_all_protozoa_)
	{
		protozoa.update();
	}
}

void World::update_debug(const sf::Vector2f mouse_position)
{
	if (selected_protozoa != nullptr)
	{
		selected_protozoa->move_selected_cell(mouse_position);
	}
}


void World::render_world()
{
	// In order to render such a large amount of organisms, we use vertex arrays, first we need to fetch the data from all protozoa.
		
	outer_color_data.clear();
	inner_color_data.clear();
	position_data.clear();

	for (Protozoa& protozoa : m_all_protozoa_)
	{
		for (Cell& cell : protozoa.get_cells())
		{
			outer_color_data.push_back(cell.outline_color_);
			inner_color_data.push_back(cell.color_);
			position_data.push_back(cell.position_);
		}
	}

	// Now we have all our data we can create the renderers and finaly render all circles
	const float radius_inner = CellSettings::cell_radius;
	const float radius_outer = radius_inner + CellSettings::cell_outline_thickness;
	const int size = position_data.size();

	outer_circle_buffer.init_texture(outer_color_data, radius_outer, size);
	inner_circle_buffer.init_texture(inner_color_data, radius_inner, size);

	outer_circle_buffer.render(position_data);
	inner_circle_buffer.render(position_data);

	// drawing the world bounds
	m_window_->draw(border_render_);
}


void World::init_organisms()
{
	m_all_protozoa_.reserve(max_protozoa);

	for (int i = 0; i < max_protozoa; ++i)
	{
		m_all_protozoa_.emplace_back(&m_bounds_, m_window_, &cell_renderer_, true);
	}
}


void World::init_food()
{

}


void World::init_environment()
{

}



void World::check_hovering(const bool debug_mode, const sf::Vector2f mouse_position)
{
	// resetting all states
	for (Protozoa& protozoa : m_all_protozoa_)
	{
		protozoa.set_debug_mode(false);
	}

	if (!debug_mode)
		return;


	for (Protozoa& protozoa : m_all_protozoa_)
	{
		if (protozoa.is_hovered_on(mouse_position))
		{
			protozoa.set_debug_mode(true);
			break;
		}
	}
}

bool World::check_pressed(const sf::Vector2f mouse_position)
{
	for (Protozoa& protozoa : m_all_protozoa_)
	{
		if (protozoa.check_press(mouse_position))
		{
			selected_protozoa = &protozoa;
			return true;
		}
	}
	return false;
}


void World::de_select_protozoa()
{
	if (selected_protozoa != nullptr)
	{
		selected_protozoa->deselect_cell();
		selected_protozoa = nullptr;
	}
}