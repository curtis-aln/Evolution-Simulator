#include "simulation.h"

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

	if (!ImGui::SFML::Init(m_window_))
	{
		std::cerr << "[ERROR]: Failed to initialize ImGui-SFML\n";
	}

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

	init_line_graphs();
	init_network_renderer();		
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


void Simulation::init_network_renderer()
{
	//net_renderer.set_title("Test Network");
	//.set_input_node_names({ "input A", "Input B" });
	//net_renderer.set_output_node_names({ "output A", "output B" });
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

		protozoa_population_graph_.handle_mouse_press(mouse_pos);
		food_population_graph_.handle_mouse_press(mouse_pos);
	}
	else if (!m_world_.paused)
	{
		m_world_.update(false);	
		update_line_graphs();
		update_test_data();
	}

	// in debug mode we want the camera to follow the selected protozoa
	if (m_world_.debug_mode)
	{
		m_world_.move_cell_in_selected_protozoa(mouse_pos);
		camera_follow_selected_protozoa();
	}

	m_builder_.update(mouse_pos);
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
	m_world_.render(&cell_statistic_font);
	
	if (!m_world_.simple_mode)
	{
		m_builder_.render();

		protozoa_population_graph_.render(camera_);
		food_population_graph_.render(camera_);
	}
}


void Simulation::handle_imGUI()
{
	ImGui::SFML::Update(m_window_, m_delta_time_.get_delta_sfml());

	// ── Simulation Stats ─────────────────────────────────────────
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(260, 0), ImGuiCond_FirstUseEver);
	ImGui::Begin("Simulation Stats");

	ImGui::SeparatorText("Performance");
	ImGui::Text("FPS:               %.1f", fps_);
	ImGui::Text("Frames:            %d", m_ticks_);
	ImGui::Text("Time Elapsed:      %.2fs", m_total_time_elapsed_);
	ImGui::Text("Paused:            %s", m_world_.paused ? "Yes" : "No");

	ImGui::SeparatorText("Population");
	ImGui::Text("Protozoa:          %d", m_world_.get_protozoa_count());
	ImGui::Text("Food:              %d", m_world_.get_food_count());
	ImGui::Text("Avg Generation:    %.2f", m_world_.get_average_generation());
	ImGui::Text("Min Speed:         %.3f", m_world_.min_speed);
	ImGui::Text("Delta Min Speed:   %.3f", m_world_.delta_min_speed);

	ImGui::End();

	// ── Controls ─────────────────────────────────────────────────
	ImGui::SetNextWindowPos(ImVec2(10, 300), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(260, 0), ImGuiCond_FirstUseEver);
	ImGui::Begin("Controls");

	ImGui::SeparatorText("Simulation");
	ImGui::BulletText("[Space]    Pause / Unpause");
	ImGui::BulletText("[R]        Toggle rendering");
	ImGui::BulletText("[O]        Step one frame (paused)");
	ImGui::BulletText("[Escape]   Quit");

	ImGui::SeparatorText("Display");
	ImGui::BulletText("[S]        Simple mode (outer cells only)");
	ImGui::BulletText("[G]        Cell collision grid overlay");
	ImGui::BulletText("[F]        Food grid overlay");
	ImGui::BulletText("[C]        Toggle collisions");

	ImGui::SeparatorText("Debug Mode  [D]");
	ImGui::BulletText("[K]        Skeleton mode");
	ImGui::BulletText("[B]        Bounding boxes");
	ImGui::BulletText("[C]        Toggle connections");

	ImGui::SeparatorText("Camera");
	ImGui::BulletText("[Scroll]   Zoom in / out");
	ImGui::BulletText("[L-Hold]   Pan camera");

	ImGui::End();

	// ── Simulation Tuning ─────────────────────────────────────────
	ImGui::SetNextWindowPos(ImVec2(10, 600), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(260, 0), ImGuiCond_FirstUseEver);
	ImGui::Begin("Tuning");

	ImGui::SeparatorText("World");
	ImGui::SliderFloat("Min Speed", &m_world_.min_speed, 0.0f, 135.0f);

	// scale up for display, scale down for storage
	float display_val = m_world_.delta_min_speed * 1000.f;
	if (ImGui::SliderFloat("Delta Min Speed (x1000)", &display_val, 0.2f, 2.0f))
		m_world_.delta_min_speed = display_val / 1000.f;

	ImGui::SeparatorText("Flags");
	ImGui::Checkbox("Rendering", &m_rendering_);
	ImGui::Checkbox("Paused", &m_world_.paused);
	ImGui::Checkbox("Debug Mode", &m_world_.debug_mode);
	ImGui::Checkbox("Simple Mode", &m_world_.simple_mode);

	ImGui::End();
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