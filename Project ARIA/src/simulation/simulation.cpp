#include "simulation.h"

inline static constexpr float lerp_factor = 0.025f; // Adjust for smoothness (0 = no movement, 1 = instant movement)


inline void draw_debug_circle(sf::RenderWindow* window, const sf::Vector2f position)
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

	constexpr float rad = WorldSettings::bounds_radius;
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
		{ 50, 50, 50, transparency }, border_outline_thickness, regular_font_size,
		regular_font_size, cell_statistic_font_size, bold_font_location, regular_font_location };

	protozoa_population_graph_.set_settings(settings);
	settings.title = "Food Population";
	settings.graph_line_color = food_graph_line_color;
	settings.under_graph_color = food_under_graph_color;
	food_population_graph_.set_settings(settings);
}

void Simulation::init_text_box()
{
	text_box.set_fonts(regular_font, cell_statistic_font);
	text_box.set_title("Protozoa Simulation");
	text_box.init_graphics(border_fill_color, border_outline_color, border_outline_thickness);

	text_box.add_statistic("int", "frames", &m_ticks_);
	text_box.add_statistic("float", "fps", &fps_);
	text_box.add_statistic("bool", "paused", &m_world_.paused);
	text_box.add_statistic("float", "time", &m_total_time_elapsed_);

	text_box.add_text("R: Toggle Rendering");
	text_box.add_text("G: toggle cell grid");
	text_box.add_text("C: toggle collisions");
	text_box.add_text("F: toggle food grid");
	text_box.add_text("S: toggle simple mode");
	text_box.add_text("O: tick frames");
	text_box.add_text("[Debug] D: debug mode");
	text_box.add_text("[Debug] K: skelleton mode");
	text_box.add_text("[Debug] C: toggle connections");
	text_box.add_text("[Debug] B: toggle bounding boxes");
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

	if (m_tick_frame_time)
	{
		m_world_.update(false);
		m_tick_frame_time = false;
	}
	else if (!m_world_.paused)
	{
		m_world_.update(false);	
	}


	if (m_debug_)
	{
		m_world_.move_cell_in_selected_protozoa(mouse_pos);
		Protozoa* selected = m_world_.get_selected_protozoa();

		if (selected != nullptr)
		{
			// comparing the difference between the camera center and the protozoa center and subtractign them to find out the camera translation
			sf::Vector2f win_center = sf::Vector2f(m_window_.getSize()) / 2.f;
			sf::Vector2f cam_center = camera_.window_pos_to_world_pos(win_center);

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
	//network.inputs[0] = std::tanh(test_data3);
	//network.inputs[1] = std::tanh(test_data4);
	//network.forward_propagate();
}


void Simulation::update_line_graphs()
{
	++m_ticks_;
	m_total_time_elapsed_ += static_cast<float>(m_delta_time_.get_delta());

	if (m_ticks_ % line_x_axis_increments == 0)
	{
		protozoa_population_graph_.add_data(m_world_.get_protozoa_count());
		food_population_graph_.add_data(m_world_.get_food_count());
	}
}


void Simulation::draw_everything()
{
	m_world_.render();
	
	if (!m_world_.simple_mode)
	{
		m_builder_.render();

		protozoa_population_graph_.render(camera_);
		food_population_graph_.render(camera_);

		net_renderer.render();
		text_box.render(camera_);
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
			if (m_world_.handle_mouse_click(cam_pos) || m_builder_.check_mouse_input())
			{
				mouse_pressed_event = true;
			}
			else
			{
				m_world_.deselect_protozoa();
			}
		}

		else if (event.type == sf::Event::MouseButtonReleased)
		{
			m_world_.de_select_protozoa();
			m_builder_.de_select_protozoa();
			mouse_pressed_event = false;
		}
	}

	if (m_world_.get_selected_protozoa() == nullptr)
	{
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !mouse_pressed_event)
		{
			camera_.translate();
		}

		// mouse hovering over a protozoa
		m_world_.check_if_mouse_is_hovering(m_debug_, cam_pos, mouse_pressed_event);
	}

	// updating camera translations
	camera_.update(m_clock_.get_delta_time());
}


void Simulation::keyboard_input(const sf::Keyboard::Key& event_key_code)
{
	switch (event_key_code) // todo move the world parts into the world class and make their respective variables private
	{
	case sf::Keyboard::Escape: running = false; break;
	case sf::Keyboard::Space:  
		m_world_.paused = not m_world_.paused; 
		
		if (!m_world_.paused)
		{
			m_tick_frame = false;
			m_tick_frame_time = false;
		}
		
		break;
	case sf::Keyboard::R:      m_rendering_ = not m_rendering_; break;
	case sf::Keyboard::G:      m_world_.draw_cell_grid = not m_world_.draw_cell_grid; break;
	
	case sf::Keyboard::C: 
		if (m_debug_)
			m_world_.show_connections = not m_world_.show_connections;
		else
			m_world_.toggle_collisions = not m_world_.toggle_collisions; 
		break;

	case sf::Keyboard::F:      m_world_.draw_food_grid = not m_world_.draw_food_grid; break;
	case sf::Keyboard::S:      m_world_.simple_mode = not m_world_.simple_mode; break;
	case sf::Keyboard::O:
		m_tick_frame_time = true;
		break;

	case sf::Keyboard::D:
		m_debug_ = not m_debug_;
		m_world_.debug_mode = not m_world_.debug_mode;
		break;
	case sf::Keyboard::K:
		if (m_debug_)
			m_world_.skeleton_mode = not m_world_.skeleton_mode; break;
		

	case sf::Keyboard::B:
		if (m_debug_)
			m_world_.show_bounding_boxes = not m_world_.show_bounding_boxes; break;
	
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
	title << simulation_name 
		<< " | FPS: " << std::fixed << std::setprecision(1) << fps_ 
		<< " | protozoa: " << m_world_.get_protozoa_count() 
		<< " | food: " << m_world_.get_food_count()
		<< " | average generation: " << m_world_.get_average_generation();
	m_window_.setTitle(title.str());
}