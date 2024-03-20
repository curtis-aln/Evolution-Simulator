#include "world.h"

World::World(sf::RenderWindow* window) : m_window(window), Sample1(world_boundaries, window, &cell_renderer)
{
	init_organisms();
	init_food();
	init_environment();

}


void World::update_world()
{
	Sample1.update();
}


void World::render_world()
{
	Sample1.render();
}


void World::render_debug()
{

}


void World::init_organisms()
{

}


void World::init_food()
{

}


void World::init_environment()
{

}



void World::check_hovering(bool debug_mode)
{
	// resetting all states
	Sample1.set_debug_mode(false);

	if (!debug_mode)
		return;

	const auto mouse_position = static_cast<sf::Vector2f>(sf::Mouse::getPosition(*m_window));

	if (Sample1.is_hovered_on(mouse_position))
	{
		Sample1.set_debug_mode(true);
	}
}