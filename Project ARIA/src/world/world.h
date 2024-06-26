#pragma once

#include <SFML/Graphics/RenderWindow.hpp>

#include "../Protozoa/Protozoa.h"

#include "../settings.h"

#include "../Utils/o_vector.hpp"
#include "../Utils/random.h"
#include "../Utils/Circle.h"

class World : WorldSettings
{
	sf::RenderWindow* m_window_ = nullptr;

	std::vector<Protozoa> m_all_protozoa_{};

	Circle m_bounds_{ {0, 0}, bounds_radius };

	sf::CircleShape cell_renderer_{};
	sf::VertexArray border_render_{};


public:
	Protozoa* selected_protozoa = nullptr;

	World(sf::RenderWindow* window = nullptr);

	void update_world();
	void update_debug(sf::Vector2f mouse_position);
	void render_world();
	void check_hovering(bool debug_mode, sf::Vector2f mouse_position);
	bool check_pressed(sf::Vector2f mouse_position);
	void de_select_protozoa();

private:
	void init_organisms();
	void init_food();
	void init_environment();
};