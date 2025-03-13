#include "world.h"
#include "../Utils/utility_SFML.h"
#include "../Utils/Graphics/buffer_renderer.h"

#include "../Utils/Graphics/spatial_hash_grid.h"

World::World(sf::RenderWindow* window)
	: m_window_(window),
	border_render_(make_circle(m_bounds_.radius, m_bounds_.center)),
	grid_renderer(*window, world_bounds, cells_x, cells_y)
{ 
	init_organisms();
	init_food();
	init_environment();

	const size_t protozoa_count = all_protozoa.size();
	const size_t predicted_cells = protozoa_count * GeneSettings::cell_amount_range.y;

	// reserving nessesary data
	outer_color_data.reserve(predicted_cells);
	inner_color_data.reserve(predicted_cells);
	position_data.reserve(predicted_cells);
}

void World::update_world()
{
	ticks++;
	food_manager.update();

	// updating the spatial grid first
	spatial_hash_grid.clear();
	for (Protozoa* protozoa : all_protozoa)
	{
		const sf::Vector2f center = protozoa->get_center();
		spatial_hash_grid.addAtom(center, protozoa->id);
	}

	for (Protozoa* protozoa : all_protozoa)
	{	
		const sf::Vector2f center = protozoa->get_center();

		Container& nearby = spatial_hash_grid.find(center);

		protozoa->update(all_protozoa, nearby);
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
	if (draw_grid)
	{
		grid_renderer.draw();
	}

	update_position_data();
	render_protozoa();

	// drawing the world bounds
	m_window_->draw(border_render_);
}

void World::render_protozoa()
{
	const float radius_inner = CellSettings::cell_radius;
	const float radius_outer = radius_inner + CellSettings::cell_outline_thickness;
	const int size = position_data.size();

	outer_circle_buffer.init_texture(outer_color_data, radius_outer, size);

	if (!simple_mode)
	{
		inner_circle_buffer.init_texture(inner_color_data, radius_inner, size);
	}

	outer_circle_buffer.render(position_data);

	if (!simple_mode)
	{
		inner_circle_buffer.render(position_data);
	}

	if (debug_mode && selected_protozoa != nullptr)
	{
		selected_protozoa->render(true);
	}
}

void World::update_position_data()
{
	outer_color_data.clear();
	inner_color_data.clear();
	position_data.clear();

	food_manager.render();

	for (Protozoa* protozoa : all_protozoa)
	{
		for (Cell& cell : protozoa->get_cells())
		{
			outer_color_data.push_back(cell.outer_color);
			inner_color_data.push_back(cell.inner_color);
			position_data.push_back(cell.position_);
		}
	}
}


void World::init_organisms()
{
	for (int i = 0; i < max_protozoa; ++i)
	{
		all_protozoa.emplace({ i, &m_bounds_, m_window_, true });
	}
	for (int i = 0; i < max_protozoa - initial_protozoa; ++i)
	{
		all_protozoa.remove(i);
	}
}


void World::init_food()
{

}


void World::init_environment()
{

}



void World::check_hovering(const bool debug_mode, const sf::Vector2f mouse_position, bool mouse_pressed)
{
	if (!debug_mode)
		return;


	for (Protozoa* protozoa : all_protozoa)
	{
		if (protozoa->is_hovered_on(mouse_position))
		{
			break; // todo
		}
	}
}

bool World::check_pressed(const sf::Vector2f mouse_position)
{
	for (Protozoa* protozoa : all_protozoa)
	{
		if (protozoa->is_hovered_on(mouse_position, true))
		{
			selected_protozoa = protozoa;
			return true;
		}
	}
	return false;
}


void World::de_select_protozoa()
{
	if (selected_protozoa != nullptr)
	{
		//selected_protozoa->deselect_cell();
		//selected_protozoa = nullptr;
	}
}