#include "world.h"
#include "../Utils/utility_SFML.h"
#include "../Utils/Graphics/CircleBatchRenderer.h"
#include "../protozoa/genetics/CellGenome.h"

World::World(sf::RenderWindow* window)
	: m_window_(window),
	world_border_renderer_(make_circle(world_circular_bounds_.radius, world_circular_bounds_.center)),
	thread_pool_(8)
{ 
	init_organisms();

	const size_t maximum_cells = max_protozoa * ProtozoaSettings::max_cells;

	// reserving nessesary data
	outer_color_data_.resize(maximum_cells);
	inner_color_data_.resize(maximum_cells);
	position_data_.resize(maximum_cells);
	radius_data_.resize(maximum_cells);
	inner_radius.resize(maximum_cells);
	distribution.reserve(maximum_cells);
	cell_pointers_.resize(maximum_cells);

	collision_resolutions.resize(maximum_cells);

	inner_circle_renderer_.init(90, maximum_cells);
	outer_circle_renderer_.init(90 + GraphicalSettings::cell_outline_thickness, maximum_cells);
}


SimpleSpatialGrid* World::get_spatial_grid()
{
	return &spatial_hash_grid_;
}

SimpleSpatialGrid* World::get_food_spatial_grid()
{
	return &food_manager_.spatial_hash_grid;
}

FoodManager* World::get_food_manager()
{
	return &food_manager_;
}	

void World::render(Font* font, sf::Vector2f mouse_pos)
{
	// In order to render such a large amount of organisms, we use vertex arrays, first we need to fetch the data from all protozoa.
	if (draw_cell_grid)
	{
		cell_grid_renderer.render_grid(*m_window_, mouse_pos, 800.f);
	}

	if (draw_food_grid)
	{
		food_manager_.draw_food_grid(mouse_pos);
	}
	
	food_manager_.render();
	render_protozoa(font);

	// drawing the world bounds
	m_window_->draw(world_border_renderer_);
}

void World::render_protozoa(Font* font)
{
	outer_circle_renderer_.update_colors(outer_color_data_, entity_count);
	outer_circle_renderer_.render(position_data_, radius_data_, entity_count);

	if (!simple_mode)
	{
		int i = 0;
		for (float& radius : radius_data_)
			inner_radius[i++] = radius / GraphicalSettings::cell_outline_thickness;

		inner_circle_renderer_.update_colors(inner_color_data_, entity_count);
		inner_circle_renderer_.render(position_data_, inner_radius, entity_count);
	}

	if (selected_protozoa_ != nullptr && debug_mode)
	{
		selected_protozoa_->render_debug(font, skeleton_mode, show_connections, show_bounding_boxes);
	}

	for (Protozoa* protozoa : all_protozoa_)
		protozoa->cell_positions_nearby.clear();
}


void World::init_organisms()
{
	// emplace creates all the actual objects of the protozoa
	for (int i = 0; i < max_protozoa; ++i)
	{
		all_protozoa_.emplace({ i, &world_circular_bounds_, m_window_ });
	}
	// we only want to start with a certain amount of protozoa so we "remove" some
	for (int i = initial_protozoa; i < max_protozoa; ++i)
	{
		all_protozoa_.at(i)->kill();
		all_protozoa_.at(i)->soft_reset();
		all_protozoa_.remove(i);
	}

	// now we grow all of these protozoa
	for (Protozoa* protozoa : all_protozoa_)
	{
		generate_protozoa(*protozoa, world_circular_bounds_);
	}
}

bool World::handle_mouse_click(const sf::Vector2f mouse_position)
{
	for (Protozoa* protozoa : all_protozoa_)
	{
		if (protozoa->check_mouse_press(mouse_position, true) != -1)
		{
			selected_protozoa_ = protozoa;
			return true;
		}
	}
	return false;
}

void World::keyboardEvents(const sf::Keyboard::Key& event_key_code)
{
	// SFML 3.x uses scoped enums: sf::Keyboard::Key::G instead of sf::Keyboard::G
	switch (event_key_code)
	{
	case sf::Keyboard::Key::G:      
		draw_cell_grid = not draw_cell_grid; 
		break;

	case sf::Keyboard::Key::C:
		if (debug_mode)
			show_connections = not show_connections;
		else
			toggle_collisions = not toggle_collisions;
		break;

	case sf::Keyboard::Key::F:      
		draw_food_grid = not draw_food_grid; 
		break;

	case sf::Keyboard::Key::S:      
		simple_mode = not simple_mode; 
		break;

	case sf::Keyboard::Key::D:
		debug_mode = not debug_mode;
		break;

	case sf::Keyboard::Key::T:      track_statistics = !track_statistics; break;

	case sf::Keyboard::Key::K:
		if (debug_mode)
			skeleton_mode = not skeleton_mode; 
		break;

	case sf::Keyboard::Key::B:
		if (debug_mode)
			show_bounding_boxes = not show_bounding_boxes; 
		break;

	default:
		break;

	}
}


const std::vector<float>& World::get_generation_distribution()
{
	distribution.clear();
	int idx = 0;
	for (Protozoa* protozoa : all_protozoa_)
	{
		for (Cell& cell : protozoa->get_cells())
		{
			distribution.push_back(cell.generation);

		}
	}

	return distribution;
}


void World::update_spatial_renderers()
{
	
	cell_grid_renderer.rebuild();
	
	food_manager_.update_food_grid_renderer();
}