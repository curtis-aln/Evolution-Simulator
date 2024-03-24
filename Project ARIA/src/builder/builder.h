#pragma once

#include <SFML/Graphics.hpp>
#include "../Utils/utility.h"
#include "../Utils/button.h"
#include "../Utils/slider.h"
#include "../Utils/font_renderer.hpp"
#include "../settings.h"

class Builder : BuilderSettings
{
	sf::RenderWindow* m_window_ = nullptr;
	sf::RectangleShape m_bounds_render_{};

	Font m_title_text_;
	Font m_normal_text_;

	Button m_button_{ {0, 0, 80, 50} };

public:
	explicit Builder(sf::RenderWindow* window);

	void render();
	void update(const sf::Vector2f mouse_pos);
	void update_buttons(const sf::Vector2f mouse_pos);

private:
	void render_box();
	void render_ui();
	void render_text();

	void init_buttons();
	void init_sliders();
	void init_bounds();
};