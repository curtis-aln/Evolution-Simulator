#include "world.h"
#include "../Utils/utility_SFML.h"

World::World(sf::RenderWindow* window) : m_window_(window), border_render_(make_circle(m_bounds_.radius, m_bounds_.center))
{
	init_organisms();
	init_food();
	init_environment();

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
	for (Protozoa& protozoa : m_all_protozoa_)
	{
		protozoa.render();
	}

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