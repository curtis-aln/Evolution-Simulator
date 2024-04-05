#include "simulation.h"
#include "../Utils/utility.h"


Simulation::Simulation() : m_world_(&m_window_)
{
	m_window_.setFramerateLimit(frame_rate);
	std::cout << "window dimensions: " << m_window_.getSize().x << " " << m_window_.getSize().y << "\n";
	m_window_.setVerticalSyncEnabled(false); // more efficient to have vsync set to false

	protozoa_population_graph_.init_graphics("Protozoa Population", "Time", "Population", 
		{ 20, 200, 20 }, { 20, 100, 20 });

	food_population_graph_.init_graphics("Food Population", "Time", "Population",
		{ 20, 200, 20 }, { 20, 100, 20 });
}


void Simulation::run()
{
	while (m_window_.isOpen())
	{
		update();
		render();
		handle_events();
	}
}


void Simulation::update()
{
	m_world_.update_world();

	if (m_debug_)
	{
		m_world_.update_debug(camera_.get_world_mouse_pos());
	}

	m_builder_.update(camera_.get_world_mouse_pos());

	update_statistics();
}


void Simulation::update_statistics()
{
	++m_ticks_;
	m_total_time_elapsed_ += static_cast<float>(m_delta_time_.get_delta());

	if (m_ticks_ % line_x_axis_increments == 0)
	{
		test_data += Random::rand_range(-2.f, 2.f);
		protozoa_population_graph_.add_data(test_data);
	}
}



void Simulation::render()
{
	m_window_.clear(window_color);

	display_screen_info();

	m_world_.render_world();
	m_builder_.render();

	protozoa_population_graph_.render(true);
	food_population_graph_.render(true);

	m_window_.display();
}


void Simulation::handle_events()
{
	camera_.update();

	sf::Event event{};
	while (m_window_.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			m_window_.close();

		else if (event.type == sf::Event::KeyPressed)
			keyboard_input(event.key.code);

		else if (event.type == sf::Event::MouseWheelScrolled)
			camera_.zoom(event.mouseWheelScroll.delta);

		else if (event.type == sf::Event::MouseButtonPressed)
			m_world_.check_pressed(camera_.get_world_mouse_pos());

		else if (event.type == sf::Event::MouseButtonReleased)
			m_world_.de_select_protazoa();
	}

	if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && m_world_.selected_protozoa == nullptr)
		camera_.translate();

	// mouse hovering over an organism
	m_world_.check_hovering(m_debug_, camera_.get_world_mouse_pos());
	
}


void Simulation::keyboard_input(const sf::Keyboard::Key& event_key_code)
{
	switch (event_key_code)
	{
	case sf::Keyboard::Escape: m_window_.close(); break;
	case sf::Keyboard::Space:  m_paused_ = not m_paused_; break;
	case sf::Keyboard::D:      m_debug_ = not m_debug_; break;

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


void Simulation::display_screen_info()
{
	Camera::toggle_view(false);
	manage_frame_rate();

	m_title_font_.draw({ 20, 20 }, simulation_name);

	const std::string combined_string = "fps: " + trim_decimal_to_string(m_clock_.get_average_frame_rate(), 2) + "\n" +
								        "time elapsed:" + trim_decimal_to_string(m_total_time_elapsed_, 2) + "s\n" +
								        "pause [SPACE]: " + bool_to_string(m_paused_) + "\n" +
								        "debug [D]: "     + bool_to_string(m_debug_) + "\n" +
								        "quit [ESC]";

	m_text_font_.draw({ 20, 55 }, combined_string);
	Camera::toggle_view(true);
}


void Simulation::manage_frame_rate()
{
	m_clock_.update_frame_rate();
}