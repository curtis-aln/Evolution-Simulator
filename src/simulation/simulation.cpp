#include "simulation.h"
#include <implot.h>


inline static constexpr float lerp_factor = 0.04f; // Adjust for smoothness (0 = no movement, 1 = instant movement)


static inline void draw_debug_circle(sf::RenderWindow* window, const sf::Vector2f position)
{
	if (!window) return; // Ensure the window pointer is valid

	sf::CircleShape circle(60); // Small debug circle
	circle.setFillColor(sf::Color::Red); // Red for visibility
	circle.setOrigin({ 5.f, 5.f }); // Center the origin
	circle.setPosition(position); // Set position

	window->draw(circle); // Draw the circle to the window
}

Simulation::Simulation() : m_world_(&m_window_)
{

	m_window_.setFramerateLimit(frame_rate);
	m_window_.setVerticalSyncEnabled(vsync);


	// center the view on the world
	constexpr float rad = WorldSettings::bounds_radius;
	camera_.m_view_.setCenter({ rad, rad });
	camera_.update_window_view();

	title_font.set_render_window(&m_window_); 
	regular_font.set_render_window(&m_window_);
	cell_statistic_font.set_render_window(&m_window_);

	title_font.set_font_size(title_font_size);
	regular_font.set_font_size(regular_font_size);
	cell_statistic_font.set_font_size(cell_statistic_font_size);

	init_imGUI();
}



void Simulation::run_simulation()
{
	while (running)
	{
		handle_events();

		update_one_frame();
		
		render();
		
	}

	ImGui::SFML::Shutdown();
	ImPlot::DestroyContext();
}

void Simulation::update_one_frame()
{
	
	manage_frame_rate();
	const sf::Vector2f mouse_pos = camera_.get_world_mouse_pos();

	if (m_tick_frame_time)
	{
		m_world_.update();
		m_tick_frame_time = false;
	}
	else if (!m_world_.paused)
	{
		m_world_.update();

		if (m_world_.track_statistics)
			update_line_graphs();
	}

	camera_follow_selected_protozoa();
	handle_imGUI();
}


void Simulation::camera_follow_selected_protozoa()
{
    Protozoa* selected = m_world_.get_selected_protozoa();
    if (selected == nullptr)
        return;

    const sf::Rect<float> bounds = selected->get_bounds();
    const sf::Vector2f target = bounds.position + bounds.size / 2.f;

    const sf::Vector2f current = camera_.m_view_.getCenter();
    const sf::Vector2f new_center = current + (target - current) * lerp_factor;

    camera_.m_view_.setCenter(new_center);
    camera_.update_window_view();
}




void Simulation::update_line_graphs()
{
	++m_ticks_;
	m_total_time_elapsed_ += static_cast<float>(m_delta_time_.get_delta());

	m_history_.push(
		m_total_time_elapsed_,
		m_world_.get_protozoa_count(),
		m_world_.get_food_count(),
		m_world_.average_generation_);

	m_history_.push_misc(
		m_world_.average_mutation_rate_,
		m_world_.average_mutation_range_,
		m_world_.average_offspring_count_,
		m_world_.average_lifetime_,
		m_world_.average_cells_per_protozoa_,
		m_world_.average_spring_count_,
		m_world_.average_energy_);
}


void Simulation::render()
{
	m_window_.clear(GraphicalSettings::window_color);

	if (m_rendering_)
	{
		const sf::Vector2f pos = camera_.get_world_mouse_pos();
		m_world_.render(&cell_statistic_font, pos);
	}

	if (!hide_panels)
		ImGui::SFML::Render(m_window_);
	else
	{
		// prevent imgui from crashing
		ImGui::EndFrame();
	}

	m_window_.display();
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