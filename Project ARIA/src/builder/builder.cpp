#include "builder.h"


Builder::Builder(sf::RenderWindow* window) : m_window_(window), m_protozoa_(nullptr, window)
{
	init_bounds();
	init_protozoa();
	init_add_button();
	init_debug_button();
	init_sliders();
	init_text_packets();
}


void Builder::add_cell()
{
	m_protozoa_.builder_add_cell(m_protozoa_bounds_.center);
}

bool Builder::check_mouse_input()
{
	return check_protozoa_interaction();
}


// -------- Updating -------- //


void Builder::update(const sf::Vector2f mouse_pos)
{
	m_mouse_pos = mouse_pos;
	update_protozoa();
	check_button_presses();
	
}

void Builder::check_button_presses()
{
	if (m_add_cell_button.check_click(m_mouse_pos))
	{
		add_cell();
	}

	if (debug_toggle.check_click(m_mouse_pos))
	{
		debug_mode_ = not debug_mode_;
	}
}


void Builder::de_select_protozoa()
{
	m_protozoa_.deselect_cell();

	if (start_cell != nullptr)
	{
		const Cell* end_cell = m_protozoa_.get_selected_cell(m_mouse_pos);

		if (end_cell != nullptr)
		{
			m_protozoa_.make_connection(start_cell->id, end_cell->id);
		}

		start_cell = nullptr;
	}
}


bool Builder::check_protozoa_interaction()
{
	// moving a cell around
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		return m_protozoa_.check_press(m_mouse_pos);
	}

	// making a connection
	if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
	{
		start_cell = m_protozoa_.get_selected_cell(m_mouse_pos);
	}

	return start_cell != nullptr;
}


void Builder::update_protozoa()
{
	m_protozoa_.update();
	m_protozoa_.move_selected_cell(m_mouse_pos);

	for (Cell& cell : m_protozoa_.get_cells())
	{
		border_cell(cell);
	}
}


void Builder::border_cell(Cell& cell) const
{
	const float rad = cell.radius;
	if (cell.position_.x - rad < protozoa_space.left)
	{
		cell.position_.x = protozoa_space.left + rad;
		//velocity.x *= -1.f;
	}

	else if (cell.position_.x + rad > protozoa_space.left + protozoa_space.width)
	{
		cell.position_.x = protozoa_space.left + protozoa_space.width - rad;
		//velocity.x *= -1.f;
	}

	if (cell.position_.y - rad < protozoa_space.top)
	{
		cell.position_.y = protozoa_space.top + rad;
		//velocity.y *= -1.f;
	}

	else if (cell.position_.y + rad > protozoa_space.top + protozoa_space.height)
	{
		cell.position_.y = protozoa_space.top + protozoa_space.height - rad;
		//velocity.y *= -1.f;
	}

	//cell.set_velocity(velocity);
}


// -------- Rendering -------- //

void Builder::render()
{
	render_box();
	render_ui();
	render_text();
	render_protozoa();
}

void Builder::render_box() const
{
	m_window_->draw(m_bounds_render_);
	draw_rect_outline(protozoa_space, *m_window_);
}


void Builder::render_ui()
{
	m_add_cell_button.draw();
	debug_toggle.draw();
}


void Builder::render_text()
{
	title_font.draw(title);
	regular_font.draw(instructions);
}


void Builder::render_protozoa()
{
	m_protozoa_.render();

	if (start_cell != nullptr)
	{
		draw_thick_line(*m_window_, start_cell->position_, m_mouse_pos, 4, 0, sf::Color::White);
	}
}



// -------- Init -------- //
void Builder::init_text_packets()
{
	const sf::Vector2f bounds_pos = bounds.getPosition();
	const sf::Vector2f bounds_size = bounds.getSize();

	title = {
		{ bounds_pos.x + bounds_size.x / 2, bounds_pos.y + title_buffer.y },
		box_title, 0,
		true
	};


	const sf::Vector2f text_size = regular_font.get_text_size(box_instructions);
	const sf::Vector2f instructions_pos = {
		bounds_pos.x + bounds_size.x - instructions_buffer.x - text_size.x,
		bounds_pos.y + bounds_size.y - instructions_buffer.y - text_size.y};

	instructions = {
		instructions_pos,
		box_instructions, 0,
		false
	};
}


void Builder::init_add_button()
{
	const sf::Rect rect = { bounds.left + button_buffer.x, bounds.top + bounds.height - button_size.y - button_buffer.y,
		button_size.x, button_size.y };

	const sf::Color text_color = { 255, 255, 255 };
	const sf::Color default_color = { 30, 30, 30 };
	const sf::Color active_color = { 50, 50, 50 };
	const sf::Color outline_color = { 200, 200, 200 };

	m_add_cell_button = Button(m_window_, rect);
	m_add_cell_button.init_font("add cell", bold_font_location, text_color, b_font_size);
	m_add_cell_button.init_graphics(default_color, active_color, border_outline_color, b_thickness);
}


void Builder::init_debug_button()
{
	sf::Rect<float> rect = m_add_cell_button.get_rect();
	rect.left += rect.width + button_spacing;

	debug_toggle = Button(m_window_, rect);
	debug_toggle.init_font("Debug Mode", bold_font_location, sf::Color::White, b_font_size);
	debug_toggle.init_graphics(b_default_color, b_active_color, border_outline_color, b_thickness);
}


void Builder::init_sliders()
{

}


void Builder::init_bounds()
{
	protozoa_space = bounds;
	protozoa_space = resize_rect(protozoa_space, { 75, 135 });
	protozoa_space.top -= 50.f;

	m_bounds_render_.setPosition(bounds.getPosition());
	m_bounds_render_.setSize(bounds.getSize());
	m_bounds_render_.setFillColor(border_fill_color);
	m_bounds_render_.setOutlineColor(border_outline_color);
	m_bounds_render_.setOutlineThickness(border_outline_thickness);
}


void Builder::init_protozoa()
{
	m_protozoa_bounds_.radius = protozoa_space.width / 1.3f;
	m_protozoa_bounds_.center = get_rect_center(protozoa_space);
	m_protozoa_.set_bounds(&m_protozoa_bounds_);
}