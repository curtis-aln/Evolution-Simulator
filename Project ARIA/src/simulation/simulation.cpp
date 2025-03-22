#include "simulation.h"
#include "../Utils/utility.h"

inline static constexpr float lerp_factor = 0.025f; // Adjust for smoothness (0 = no movement, 1 = instant movement)


inline void draw_debug_circle(sf::RenderWindow* window, sf::Vector2f position)
{
	if (!window) return; // Ensure the window pointer is valid

	sf::CircleShape circle(60); // Small debug circle
	circle.setFillColor(sf::Color::Red); // Red for visibility
	circle.setOrigin(5.f, 5.f); // Center the origin
	circle.setPosition(position); // Set position

	window->draw(circle); // Draw the circle to the window
}

Simulation::Simulation() : m_world_(&m_window_)
{
	m_window_.setFramerateLimit(frame_rate);
	m_window_.setVerticalSyncEnabled(vsync);

	const float rad = WorldSettings::bounds_radius;
	camera_.translate({ -rad, -rad });
	camera_.zoom(-1000);
	camera_.update_window_view();

	title_font.set_render_window(&m_window_); 
	regular_font.set_render_window(&m_window_);
	cell_statistic_font.set_render_window(&m_window_);

	title_font.set_font_size(title_font_size);
	regular_font.set_font_size(regular_font_size);
	cell_statistic_font.set_font_size(cell_statistic_font_size);

	init_line_graphs();
	init_network_renderer();
	init_text_box();
		
}

void Simulation::init_line_graphs()
{
	LineGraphSettings settings = {
		"Protozoa Population", "Time", "Population",
		transparency, protozoa_graph_line_color, protozoa_under_graph_color, border_fill_color, border_outline_color,
		{ 50, 50, 50, transparency }, border_outline_thickness, title_font_size,
		regular_font_size, cell_statistic_font_size, bold_font_location, regular_font_location };

	protozoa_population_graph_.set_settings(settings);
	settings.title = "Food Population";
	settings.graph_line_color = food_graph_line_color;
	settings.under_graph_color = food_under_graph_color;
	food_population_graph_.set_settings(settings);
}

void Simulation::init_text_box()
{
	text_box.set_title("Protozoa Simulation");
	text_box.init_graphics(border_fill_color, border_outline_color, border_outline_thickness);

	text_box.add_statistic("int", "frames", &m_ticks_);
	text_box.add_statistic("float", "fps", &fps_);
	text_box.add_statistic("bool", "paused", &m_world_.paused);
	text_box.add_statistic("float", "time", &m_total_time_elapsed_);
}

void Simulation::init_network_renderer()
{
	net_renderer.set_title("Test Network");
	net_renderer.set_input_node_names({ "input A", "Input B" });
	net_renderer.set_output_node_names({ "output A", "output B" });
}


void Simulation::run()
{
	render_loop();

}


void Simulation::render_loop()
{
	while (running)
	{
		handle_events();

		update_one_frame();

		if (m_rendering_)
		{
			render();
		}
	}
}

void Simulation::update_one_frame()
{
	manage_frame_rate();
	const sf::Vector2f mouse_pos = camera_.get_world_mouse_pos();

	m_world_.update_world();

	if (m_debug_)
	{
		m_world_.update_debug(mouse_pos);
		Protozoa* selected = m_world_.selected_protozoa;

		camera_following_ = selected != nullptr;
		if (selected != nullptr)
		{
			// comparing the difference between the camera center and the protozoa center and subtractign them to find out the camera translation
			sf::Vector2f win_center = sf::Vector2f(m_window_.getSize()) / 2.f;
			sf::Vector2f cam_center = camera_.map_window_position_to_world_position(win_center);

			sf::Rect<float> bounds = selected->get_bounds();
			sf::Vector2f center = bounds.getPosition() + bounds.getSize() / 2.f;
			
			sf::Vector2f new_position = cam_center + (cam_center - center) * lerp_factor;
			sf::Vector2f translation = new_position - cam_center;

			camera_.translate(translation);

		}
	}

	m_builder_.update(mouse_pos);
	protozoa_population_graph_.update(mouse_pos);
	food_population_graph_.update(mouse_pos);
	net_renderer.update(mouse_pos);

	update_line_graphs();
	update_test_data();
}


void Simulation::update_test_data()
{
	test_data3 += Random::rand11_float() * 0.05f;
	test_data4 += Random::rand11_float() * 0.05f;

	test_data3 = std::clamp(test_data3, -1.f, 1.f);
	test_data4 = std::clamp(test_data4, -1.f, 1.f);

	network.inputs[0] = std::tanh(test_data3);
	network.inputs[1] = std::tanh(test_data4);
	network.forward_propagate();
}


void Simulation::update_line_graphs()
{
	++m_ticks_;
	m_total_time_elapsed_ += static_cast<float>(m_delta_time_.get_delta());

	if (m_ticks_ % line_x_axis_increments == 0)
	{
		test_data += Random::rand_range(-2.f, 2.f);
		test_data2 += Random::rand_range(-2.f, 2.f);
		protozoa_population_graph_.add_data(test_data);
		food_population_graph_.add_data(test_data2);
	}
}


void Simulation::draw_everything()
{
	m_world_.render_world();
	
	if (!m_world_.simple_mode)
	{
		m_builder_.render();

		protozoa_population_graph_.render();
		food_population_graph_.render();

		net_renderer.render();
		text_box.render();
	}
}

void Simulation::render()
{
	m_window_.clear(window_color);

	draw_everything();

	m_window_.display();
}


void Simulation::handle_events()
{
	const sf::Vector2f cam_pos = camera_.get_world_mouse_pos();

	sf::Event event{};
	while (m_window_.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			running = false;

		else if (event.type == sf::Event::KeyPressed)
			keyboard_input(event.key.code);

		else if (event.type == sf::Event::MouseWheelScrolled)
			camera_.zoom(event.mouseWheelScroll.delta);

		// if the mouse button is pressed, we see if it pressed on a protozoa bounding box. if so then we have it selected
		else if (event.type == sf::Event::MouseButtonPressed)
		{
			if (m_world_.check_pressed(cam_pos) || m_builder_.check_mouse_input())
			{
				mouse_pressed_event = true;
			}
			else
			{
				m_world_.selected_protozoa = nullptr;
			}
		}

		else if (event.type == sf::Event::MouseButtonReleased)
		{
			m_world_.de_select_protozoa();
			m_builder_.de_select_protozoa();
			mouse_pressed_event = false;
		}
	}

	if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !mouse_pressed_event && !camera_following_)
	{
		camera_.translate();
	}

	// mouse hovering over an organism
	m_world_.check_hovering(m_debug_, cam_pos, mouse_pressed_event);

	// updating camera translations
	camera_.update(m_clock_.get_delta_time());
}


void Simulation::keyboard_input(const sf::Keyboard::Key& event_key_code)
{
	switch (event_key_code) // todo move the world parts into the world class and make their respective variables private
	{
	case sf::Keyboard::Escape: running = false; break;
	case sf::Keyboard::Space:  m_world_.paused = not m_world_.paused; break;
	case sf::Keyboard::R:      m_rendering_ = not m_rendering_; break;
	case sf::Keyboard::G:      m_world_.draw_cell_grid = not m_world_.draw_cell_grid; break;
	case sf::Keyboard::K:      m_world_.skeleton_mode = not m_world_.skeleton_mode; break;
	case sf::Keyboard::F:      m_world_.draw_food_grid = not m_world_.draw_food_grid; break;
	case sf::Keyboard::D:      
		m_debug_ = not m_debug_; 
		m_world_.debug_mode = not m_world_.debug_mode;
		break;

	case sf::Keyboard::S:      m_world_.simple_mode = not m_world_.simple_mode; break;
	// todo
	//case sf::Keyboard::Key::S:
	//	if (shifting)
	//		saveData();
	//	break;
	//
	//case sf::Keyboard::Key::L:
	//	if (shifting)
	//		loadData();
	//	break;


	default:
		break;

	}
}


void Simulation::mouse_input()
{

}



void Simulation::manage_frame_rate()
{
	fps_ = static_cast<float>(m_clock_.get_average_frame_rate());
	m_clock_.update_frame_rate();

	// Display FPS in the title bar
	std::ostringstream title;
	title << simulation_name << " | FPS: " << std::fixed << std::setprecision(1) << fps_ << " | protozoa: " << m_world_.get_protozoa_count() << " | food: " << m_world_.get_food_count();
	m_window_.setTitle(title.str());
}