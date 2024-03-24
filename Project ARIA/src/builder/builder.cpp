#include "builder.h"


Builder::Builder(sf::RenderWindow* window) : m_window_(window),
m_title_text_(window, 25, "src/Utils/fonts/Roboto-Bold.ttf"),
m_normal_text_(window, 15, "src/Utils/fonts/Roboto-Regular.ttf")
{
	init_bounds();
	init_buttons();
	init_sliders();
}


void Builder::init_buttons()
{
	m_button_.init_font("click me", "src/Utils/fonts/Roboto-Regular.ttf", 15, { 60, 60, 60 });
	m_button_.set_position(bounds.getPosition() + sf::Vector2f(20, 250));
}


void Builder::init_sliders()
{

}


void Builder::init_bounds()
{
	m_bounds_render_.setPosition(bounds.getPosition());
	m_bounds_render_.setSize(bounds.getSize());
	m_bounds_render_.setFillColor(fill_color);
	m_bounds_render_.setOutlineColor(outline_color);
	m_bounds_render_.setOutlineThickness(4);
}


void Builder::render()
{
	render_box();
	render_ui();
	render_text();
}


void Builder::update(const sf::Vector2f mouse_pos)
{
	update_buttons(mouse_pos);
}

void Builder::update_buttons(const sf::Vector2f mouse_pos)
{
	if (m_button_.check_click(mouse_pos))
	{
		std::cout << "Hello World!\n";
	}
}


void Builder::render_box()
{
	m_window_->draw(m_bounds_render_);
}


void Builder::render_ui()
{
	m_button_.draw(*m_window_);
}


void Builder::render_text()
{
	const sf::Vector2f bounds_pos = bounds.getPosition();
	const sf::Vector2f bounds_size = bounds.getSize();
	m_title_text_.draw({ bounds_pos.x + bounds_size.x / 2, bounds_pos.y + 20 }, "Builder Box", true);
}
