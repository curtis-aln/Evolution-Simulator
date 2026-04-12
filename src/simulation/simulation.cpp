#include "simulation.h"
#include <implot.h>

inline static constexpr float lerp_factor = 0.04f;

Simulation::Simulation() : m_world_(&m_window_)
{
    m_window_.setFramerateLimit(frame_rate);
    m_window_.setVerticalSyncEnabled(vsync);

    const float rad = WorldSettings::bounds_radius;
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

    if (m_world_.toggles.m_tick_frame_time)
    {
        m_world_.unload_render_data(shared_input.buffers[shared_input.get_read_buffer_index()]);
        m_world_.update();
        m_world_.toggles.m_tick_frame_time = false;
    }
    else if (!m_world_.toggles.paused)
    {
        m_world_.update();
        fill_render_data(shared_output);            
    }

    camera_follow_selected_protozoa();
    
}

void Simulation::camera_follow_selected_protozoa()
{
    Protozoa* selected = m_world_.get_selected_protozoa();
    if (selected == nullptr)
        return;

    const sf::Rect<float> bounds = selected->get_bounds();
    const sf::Vector2f    target = bounds.position + bounds.size / 2.f;
    const sf::Vector2f    current = camera_.m_view_.getCenter();

    camera_.m_view_.setCenter(current + (target - current) * lerp_factor);
    camera_.update_window_view();
}

void Simulation::update_line_graphs(SimSnapshot& snapshot)
{
    ++m_ticks_;
    m_total_time_elapsed_ += static_cast<float>(m_delta_time_.get_delta());

    m_history_.push(
        m_total_time_elapsed_,
        snapshot.stats.protozoa_count,
        snapshot.stats.food_count,
        snapshot.stats.average_generation);

    m_history_.push_misc(
        snapshot.stats.average_mutation_rate,
        snapshot.stats.average_mutation_range,
        snapshot.stats.average_offspring_count,
        snapshot.stats.average_lifetime,
        snapshot.stats.average_cells_per_protozoa,
        snapshot.stats.average_spring_count,
        snapshot.stats.average_energy);
}

void Simulation::render()
{
	const int read_idx = shared_output.get_read_buffer_index();
    const int write_idx = shared_input.get_write_buffer_index();

    // moving the world render data calculated by the update into the shared input so it can get modified by Imgui
	shared_input.buffers[write_idx].render = shared_output.buffers[read_idx].render;

	update_line_graphs(shared_input.buffers[write_idx]);
    handle_imGUI();

	shared_input.publish_write();

    m_window_.clear(GraphicalSettings::window_color);

    if (m_world_.toggles.m_rendering_)
    {
        const sf::Vector2f pos = camera_.get_world_mouse_pos();
        m_world_.render(&cell_statistic_font, pos);
    }

    // ImGui must always be rendered — suppressing Render() corrupts the context.
    // Hide panels by simply not building any ImGui windows in handle_imGUI().
    ImGui::SFML::Render(m_window_);

    m_window_.display();
}

void Simulation::manage_frame_rate()
{
    fps_ = static_cast<float>(m_clock_.get_average_frame_rate());
    m_clock_.update_frame_rate();

    const WorldStatistics& stats = m_world_.get_statistics();

    std::ostringstream title;
    title << simulation_name
        << " | FPS: " << std::fixed << std::setprecision(1) << fps_
        << " | protozoa: " << stats.protozoa_count
        << " | food: " << m_world_.get_food_count()
        << " | average generation: " << stats.average_generation;
    m_window_.setTitle(title.str());
}

void Simulation::fill_render_data(SharedState& shared_state)
{
    const int    write_idx = shared_state.get_write_buffer_index();
    SimSnapshot& snapshot = shared_state.buffers[write_idx];

	m_world_.fill_render_data(snapshot);
    snapshot.total_time_elapsed = m_total_time_elapsed_;

    shared_state.publish_write();
}