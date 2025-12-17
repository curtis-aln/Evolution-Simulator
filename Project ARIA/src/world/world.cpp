#include "world.h"
#include "../Utils/utility_SFML.h"
#include "../Utils/Graphics/CircleBatchRenderer.h"

World::World(sf::RenderWindow* window)
	: m_window_(window),
	world_border_renderer_(make_circle(world_circular_bounds_.radius, world_circular_bounds_.center)),
	cell_grid_renderer(*window, world_rect_bounds_, cells_x, cells_y),
	thread_pool_(8)
{ 
	init_organisms();
	init_food();
	init_environment();

	global_cell_vector_.reserve(max_protozoa * GeneSettings::cell_amount_range.y);

	const size_t protozoa_count = all_protozoa_.size();
	const size_t predicted_cells = protozoa_count * GeneSettings::cell_amount_range.y;

	// reserving nessesary data
	outer_color_data_.reserve(predicted_cells);
	inner_color_data_.reserve(predicted_cells);
	position_data_.reserve(predicted_cells);
}

void World::move_cell_in_selected_protozoa(const sf::Vector2f mouse_position) const
{
	if (selected_protozoa_ != nullptr)
	{
		selected_protozoa_->move_selected_cell(mouse_position);
	}
}


void World::render()
{
	// In order to render such a large amount of organisms, we use vertex arrays, first we need to fetch the data from all protozoa.
	if (draw_cell_grid)
	{
		cell_grid_renderer.draw();
	}

	if (draw_food_grid)
	{
		food_manager_.draw_food_grid();
	}

	update_position_container();
	render_protozoa();

	// drawing the world bounds
	m_window_->draw(world_border_renderer_);
}

void World::render_protozoa()
{
	constexpr float radius_inner = CellSettings::cell_radius;
	const float radius_outer = radius_inner + CellSettings::cell_outline_thickness;
	const int size = position_data_.size();

	outer_circle_renderer_.init_texture(outer_color_data_, radius_outer, size);

	// if simple mode is off then we init the inner circle texture
	if (!simple_mode)
	{
		inner_circle_renderer_.init_texture(inner_color_data_, radius_inner, size);
	}

	// we want the springs to be rendered behind the cells
	if (selected_protozoa_ != nullptr && debug_mode && !skeleton_mode)
	{
		selected_protozoa_->render_protozoa_springs();

		
	}

	// rendering the rest of the protozoas
	outer_circle_renderer_.render(position_data_);

	if (!simple_mode)
	{
		inner_circle_renderer_.render(position_data_);
	}

	// if our selected cell needs debugging
	if (debug_mode && selected_protozoa_ != nullptr)
	{
		selected_protozoa_->render_debug(skeleton_mode, show_connections, show_bounding_boxes);
	}

	// clearing all of their nearby data to be re-written to
	for (Protozoa* protozoa : all_protozoa_)
	{
		protozoa->cell_positions_nearby.clear();
	}
}


void World::init_organisms()
{
	// emplace creates all the actual objects of the protozoa
	for (int i = 0; i < max_protozoa; ++i)
	{
		all_protozoa_.emplace({ i, &world_circular_bounds_, m_window_, true });
	}
	// we only want to start with a certain amount of protozoa so we "remove" some
	for (int i = 0; i < max_protozoa - initial_protozoa; ++i)
	{
		all_protozoa_.remove(i);
	}
}


void World::init_food()
{

}

void World::init_environment()
{

}

void World::check_if_mouse_is_hovering(const sf::Vector2f mouse_position, bool mouse_pressed) const
{
	if (!debug_mode)
		return;


	for (Protozoa* protozoa : all_protozoa_)
	{
		if (protozoa->is_hovered_on(mouse_position))
		{
			break; // todo
		}
	}
}

bool World::handle_mouse_click(const sf::Vector2f mouse_position)
{
	for (Protozoa* protozoa : all_protozoa_)
	{
		if (protozoa->is_hovered_on(mouse_position, true))
		{
			selected_protozoa_ = protozoa;
			return true;
		}
	}
	return false;
}

void World::keyboardEvents(const sf::Keyboard::Key& event_key_code)
{
	
	switch (event_key_code) // todo move the world parts into the world class and make their respective variables private
	{
	case sf::Keyboard::G:      
		draw_cell_grid = not draw_cell_grid; 
		break;

	case sf::Keyboard::C:
		if (debug_mode)
			show_connections = not show_connections;
		else
			toggle_collisions = not toggle_collisions;
		break;

	case sf::Keyboard::F:      
		draw_food_grid = not draw_food_grid; 
		break;

	case sf::Keyboard::S:      
		simple_mode = not simple_mode; 
		break;

	case sf::Keyboard::D:
		debug_mode = not debug_mode;
		break;

	case sf::Keyboard::K:
		if (debug_mode)
			skeleton_mode = not skeleton_mode; 
		break;

	case sf::Keyboard::B:
		if (debug_mode)
			show_bounding_boxes = not show_bounding_boxes; 
		break;

	default:
		break;

	}
}