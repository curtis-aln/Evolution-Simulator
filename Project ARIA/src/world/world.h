#pragma once

#include <SFML/Graphics/RenderWindow.hpp>

#include "../Protozoa/Protozoa.h"

#include "../settings.h"

#include "../Utils/o_vector.hpp"
#include "../Utils/random.h"

class World : WorldSettings
{
	sf::RenderWindow* m_window = nullptr;

	// using one instance of the cell renderer for all cells
	sf::CircleShape cell_renderer{};

	Protozoa Sample1{};

public:
	World(sf::RenderWindow* window = nullptr);

	void update_world();
	void render_world();
	void render_debug();
	void check_hovering(bool debug_mode);

private:
	void init_organisms();
	void init_food();
	void init_environment();
};