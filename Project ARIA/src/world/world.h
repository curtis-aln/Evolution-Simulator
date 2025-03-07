#pragma once

#include <SFML/Graphics/RenderWindow.hpp>

#include "../Protozoa/Protozoa.h"

#include "../settings.h"

#include "../Utils/o_vector.hpp"
#include "../Utils/random.h"
#include "../Utils/Circle.h"
#include "../Utils/buffer_renderer.h"


class World : WorldSettings
{
	sf::RenderWindow* m_window_ = nullptr;

	std::vector<Protozoa> m_all_protozoa_{};

	Circle m_bounds_{ {0, 0}, bounds_radius };

	sf::CircleShape cell_renderer_{};
	sf::VertexArray border_render_{};

	// rendering
	std::vector<sf::Color> outer_color_data{};
	std::vector<sf::Color> inner_color_data{};
	std::vector<sf::Vector2f> position_data{};

	CircleBuffer outer_circle_buffer{ m_window_ };
	CircleBuffer inner_circle_buffer{ m_window_ };

public:
	bool simple_mode = false;
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