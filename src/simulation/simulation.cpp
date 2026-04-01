#include "simulation.h"
#include <implot.h>


inline static constexpr float lerp_factor = 0.025f; // Adjust for smoothness (0 = no movement, 1 = instant movement)


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

	init_imGUI();

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
}



void Simulation::run_simulation()
{
	while (running)
	{
		handle_events();

		update_one_frame();


		if (m_rendering_)
		{
			render();
		}
		else
		{
			ImGui::EndFrame(); // pair with the Update() call in handle_imGUI()
		}
	}

	ImGui::SFML::Shutdown();
	ImPlot::DestroyContext();
}

void Simulation::update_one_frame()
{
	handle_imGUI();
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
		update_line_graphs();
	}

	// in debug mode we want the camera to follow the selected protozoa
	if (m_world_.debug_mode)
	{
		camera_follow_selected_protozoa();
	}
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

	if (m_ticks_ % line_x_axis_increments != 0)
		return;
	
	time_history_.push_back(m_total_time_elapsed_);
	protozoa_history_.push_back(static_cast<float>(m_world_.get_protozoa_count()));
	food_history_.push_back(static_cast<float>(m_world_.get_food_count()));

	bool graph_full = static_cast<int>(time_history_.size()) > graph_history;
	if (!graph_full)
		return;

	time_history_.erase(time_history_.begin());
	protozoa_history_.erase(protozoa_history_.begin());
	food_history_.erase(food_history_.begin());
	
}


void Simulation::draw_everything()
{
	m_world_.render(&cell_statistic_font);
}




void Simulation::render()
{
	m_window_.clear(window_color);

	draw_everything();

	ImGui::SFML::Render(m_window_);

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