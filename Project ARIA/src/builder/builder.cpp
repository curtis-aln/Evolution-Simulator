#include "builder.h"


Builder::Builder(sf::RenderWindow* window) : m_window_(window),
m_title_text_(window, 25, "src/Utils/fonts/Roboto-Bold.ttf"),
m_normal_text_(window, 15, "src/Utils/fonts/Roboto-Regular.ttf")
{
	init_bounds();
	init_protozoa();
	init_buttons();
	init_sliders();
}


void Builder::init_buttons()
{
	const sf::Vector2f size = { 100, 50 };
	const sf::Vector2f buffer = { 50, 20 };
	const sf::Rect rect = { bounds.left + buffer.x, bounds.top + bounds.height - size.y - buffer.y,
		size.x, size.y };

	m_add_cell_button.set_rect(rect);
	m_add_cell_button.init_font("Add Cell", 25);
	m_add_cell_button.set_rest_color({ 10, 10, 10 });
	m_add_cell_button.set_rest_color({ 50, 170, 50 });
}


void Builder::init_sliders()
{

}


void Builder::init_bounds()
{
	const sf::Vector2f resize = { 25, 60 };
	protozoa_space = resize_rect(bounds, resize);
	protozoa_space.top -= resize.y/3.f;


	m_bounds_render_.setPosition(bounds.getPosition());
	m_bounds_render_.setSize(bounds.getSize());
	m_bounds_render_.setFillColor(fill_color);
	m_bounds_render_.setOutlineColor(outline_color);
	m_bounds_render_.setOutlineThickness(4);
}

void Builder::init_protozoa()
{
	m_protozoa_bounds_.radius = protozoa_space.height / 2;
	m_protozoa_bounds_.center = get_rect_center(protozoa_space);
	m_protozoa_ = Protozoa(&m_protozoa_bounds_, m_window_, &m_protozoa_renderer_, false);
}


void Builder::render()
{
	render_box();
	render_ui();
	render_text();
	render_protozoa();
}


void Builder::update(const sf::Vector2f mouse_pos)
{
	update_buttons(mouse_pos);
	update_protozoa();
}

void Builder::update_buttons(const sf::Vector2f mouse_pos)
{
	if (m_add_cell_button.check_click(mouse_pos))
	{
		add_cell();
	}
}

void Builder::update_protozoa()
{
	m_protozoa_.update();
}

void Builder::add_cell()
{
	m_protozoa_.builder_add_cell(m_protozoa_bounds_.center);
}


void Builder::render_box()
{
	m_window_->draw(m_bounds_render_);
	draw_rect_outline(protozoa_space, *m_window_);
}


void Builder::render_ui()
{
	m_add_cell_button.draw();
}


void Builder::render_text()
{
	const sf::Vector2f bounds_pos = bounds.getPosition();
	const sf::Vector2f bounds_size = bounds.getSize();
	m_title_text_.draw({ bounds_pos.x + bounds_size.x / 2, bounds_pos.y + 20 }, "Builder Box", true);
}


void Builder::render_protozoa()
{
	m_protozoa_.render();
}