#pragma once

#include <SFML/Graphics.hpp>
#include "../Utils/utility.h"
#include "../Utils/utility_SFML.h"
#include "../Utils/button.h"
#include "../Utils/slider.h"
#include "../Utils/font_renderer.hpp"
#include "../settings.h"

#include "../Protozoa/Protozoa.h"


class Builder : BuilderSettings
{
	sf::RenderWindow* m_window_ = nullptr;
	sf::RectangleShape m_bounds_render_{};

	sf::Rect<float> protozoa_space;

	Font m_title_text_;
	Font m_normal_text_;

	Button m_add_cell_button{m_window_, "src/Utils/fonts/Roboto-Regular.ttf"};

	sf::CircleShape m_protozoa_renderer_{};
	Circle m_protozoa_bounds_{};
	Protozoa m_protozoa_;

public:
	explicit Builder(sf::RenderWindow* window);

	void render();
	void update(sf::Vector2f mouse_pos);
	void update_buttons(sf::Vector2f mouse_pos);
	void update_protozoa();
	void add_cell();

private:
	void render_box();
	void render_ui();
	void render_text();
	void render_protozoa();

	void init_buttons();
	void init_sliders();
	void init_bounds();
	void init_protozoa();
};