#include "simulation.h"

// ---- Mouse helpers ------------------------------------------------

bool Simulation::try_select_protozoa(const sf::Vector2f& cam_pos)
{
	return m_world_.handle_mouse_click(cam_pos) || m_builder_.check_mouse_input();
}

void Simulation::handle_mouse_press(const sf::Vector2f& cam_pos)
{
	if (try_select_protozoa(cam_pos))
		mouse_pressed_event = true;
	else
		m_world_.deselect_protozoa();
}

void Simulation::handle_mouse_release()
{
	m_world_.deselect_protozoa();
	m_builder_.de_select_protozoa();
	mouse_pressed_event = false;
}

void Simulation::handle_camera_and_hover(const sf::Vector2f& cam_pos)
{
	if (m_world_.get_selected_protozoa() != nullptr)
		return;

	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && !mouse_pressed_event)
		camera_.translate();

	m_world_.check_if_mouse_is_hovering(cam_pos, mouse_pressed_event);
}

// ---- Keyboard helpers ---------------------------------------------

void Simulation::handle_pause_toggle()
{
	m_world_.paused = !m_world_.paused;

	if (!m_world_.paused)
	{
		m_tick_frame = false;
		m_tick_frame_time = false;
	}
}

void Simulation::handle_keyboard_events(const sf::Keyboard::Key& event_key_code)
{
	switch (event_key_code)
	{
	case sf::Keyboard::Key::Escape: running = false;          break;
	case sf::Keyboard::Key::Space:  handle_pause_toggle();    break;
	case sf::Keyboard::Key::R:      m_rendering_ = !m_rendering_; break;
	case sf::Keyboard::Key::O:      m_tick_frame_time = true; break;
	default: break;
	}
}

// ---- Event dispatch -----------------------------------------------

void Simulation::dispatch_event(const sf::Event& event, const sf::Vector2f& cam_pos)
{
	if (event.is<sf::Event::Closed>())
		running = false;

	else if (const auto* key = event.getIf<sf::Event::KeyPressed>())
	{
		handle_keyboard_events(key->code);
		m_world_.keyboardEvents(key->code);
	}
	else if (const auto* scroll = event.getIf<sf::Event::MouseWheelScrolled>())
		camera_.zoom(scroll->delta);

	else if (event.is<sf::Event::MouseButtonPressed>())
		handle_mouse_press(cam_pos);

	else if (event.is<sf::Event::MouseButtonReleased>())
		handle_mouse_release();
}

// ---- Top-level ----------------------------------------------------

void Simulation::handle_events()
{
	const sf::Vector2f cam_pos = camera_.get_world_mouse_pos();

	while (const std::optional event = m_window_.pollEvent())
		dispatch_event(*event, cam_pos);

	handle_camera_and_hover(cam_pos);
	camera_.update(m_clock_.get_delta_time());
}