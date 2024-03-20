#include "simulation.h"


inline std::string trimDoubleToString(const float number, const int precision)
{
	// Convert the double to a string with the specified precision
	std::string result = std::to_string(number);
	const size_t dotPosition = result.find('.');

	if (dotPosition != std::string::npos)
	{
		// Check if there are enough characters after the dot
		if (result.length() - dotPosition > precision + 1)
		{
			result = result.substr(0, dotPosition + precision + 1);
		}
	}

	return result;
}

inline std::string boolToString(const bool val)
{
	if (val)
		return "True";
	return "False";
}



Simulation::Simulation() : m_World(&m_window_)
{
	m_window_.setFramerateLimit(frame_rate);
	std::cout << "window dimensions: " << m_window_.getSize().x << " " << m_window_.getSize().y << "\n";
	m_window_.setVerticalSyncEnabled(false); // more efficient to have vsync set to false
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
	++m_ticks;
	m_total_time_elapsed += static_cast<float>(deltaTime.GetDelta());

	m_World.update_world();
}



void Simulation::render()
{
	m_window_.clear(window_color);

	display_frame_rate();
	display_statistics();
	m_World.render_world();
	m_World.render_debug();

	m_window_.display();
}


void Simulation::handle_events()
{
	//const sf::Vector2f delta = updateMousePos(getMousePositionFloat(m_window));

	//if (m_mousePressed)
	//	translate(delta);

	sf::Event event{};
	while (m_window_.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			m_window_.close();

		else if (event.type == sf::Event::KeyPressed)
			keyboard_input(event.key.code);

		//else if (event.type == sf::Event::MouseWheelScrolled)
		//	zoom(event.mouseWheelScroll.delta);
		//
		//else if (event.type == sf::Event::MouseButtonPressed)
		//{
		//	if (event.mouseButton.button == sf::Mouse::Left)
		//		m_mousePressed = true;
		//
		//	else
		//	{
		//		const sf::Vector2i mousePos = sf::Mouse::getPosition(m_window);
		//		std::cout << mousePos.x << " " << mousePos.y << "\n";
		//
		//		const auto scaled_pos = static_cast<sf::Vector2f>(mousePos) * m_currentScroll;
		//
		//		std::cout << scaled_pos.x << " " << scaled_pos.y << "\n";
		//	}
		//}
		//
		//else if (event.type == sf::Event::MouseButtonReleased)
		//{
		//	if (event.mouseButton.button == sf::Mouse::Left)
		//		m_mousePressed = false;
		//}
	}

	// mouse hovering over an organism
	m_World.check_hovering(m_debug_);
	
}


void Simulation::keyboard_input(const sf::Keyboard::Key& event_key_code)
{
	const bool shifting = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift);

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

void Simulation::display_frame_rate()
{

}

void Simulation::display_statistics()
{
	manage_framerate();
	const std::string better_fps = std::to_string(m_frameRateManager.getFrameRate());
	const std::string time = trimDoubleToString(m_total_time_elapsed, 2);

	title_font.draw({ 20, 20 }, simulation_name);
	text_font.draw({ 20, 65 }, "fps: " + better_fps);
	//text_font.draw({ 20, 90 }, "frame ticks:" + m_ticks);
	text_font.draw({ 20, 115 }, "time elapsed:" + time + "s");

	text_font.draw({ 20, 245 }, "pause [SPACE]: " + boolToString(m_paused_));
	text_font.draw({ 20, 270 }, "quit [ESC]");
	text_font.draw({ 20, 295 }, "debug [D]: " + boolToString(m_debug_));
}


void Simulation::manage_framerate()
{
	const auto raw_fps = static_cast<unsigned>(1.f / m_clock_.restart().asSeconds());
	m_frameRateManager.updateFrameRates(raw_fps);
}