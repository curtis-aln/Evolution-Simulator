#pragma once

#include <SFML/Graphics.hpp>
#include "../Utils/UI/button.h"
#include "../Utils/Graphics/font_renderer.hpp"
#include "../settings.h"

#include "../Protozoa/Protozoa.h"


class Builder : BuilderSettings, ButtonSettings, TextSettings, UI_Settings
{
	sf::RenderWindow* m_window_ = nullptr;
	sf::RectangleShape m_bounds_render_{};

	sf::Rect<float> protozoa_space;

	TextPacket title{};
	TextPacket instructions{};

	Button m_add_cell_button{};

	sf::CircleShape m_protozoa_renderer_{};
	Circle m_protozoa_bounds_{};
	Protozoa m_protozoa_;

	// connections
	Cell* start_cell{};
	sf::Vector2f m_mouse_pos{};

	bool debug_mode_ = false;
	Button debug_toggle{};

	Font* title_font_;
	Font* regular_font_;
	Font* cell_statistic_font_;


public:
	explicit Builder(sf::RenderWindow* window, Font* title_font, Font* regular_font, Font* cell_statistic_font);
	void init_text_packets();

	void render();
	void update(sf::Vector2f mouse_pos);
	void add_cell() const;
	bool check_mouse_input();
	void de_select_protozoa();

private:
	void check_button_presses();
	bool check_protozoa_interaction();
	void update_protozoa();
	void border_cell(Cell& cell) const;

	void render_box() const;
	void render_ui();
	void render_text() const;
	void render_protozoa();

	void init_add_button();
	void init_debug_button();
	static void init_sliders();
	void init_bounds();
	void init_protozoa();
};