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
        update_world();
        m_world_.toggles.m_tick_frame_time = false;
    }
    else if (!m_world_.toggles.paused)
    {
        update_world();
                    
    }

    camera_follow_selected_protozoa();
    
}

void Simulation::update_world()
{
    // Apply any commands ImGui pushed since last tick
    {
		// lock the command queue while we process it, but release it before ticking the simulation
        std::lock_guard lock(m_cmd_mutex);
        while (!m_commands.empty())
        {
            const SimCommand& cmd = m_commands.front();
            switch (cmd.type)
            {
            case CommandType::SetToggles:
                m_world_.toggles = cmd.toggles;
                break;

            //case CommandType::CloneProtozoa:
                // hook up your clone logic here in stage 2
            //    break;

            case CommandType::ResetSimulation:
                // hook up your reset logic here in stage 2
                break;
            }
            m_commands.pop();
        }
    } // mutex released here

    // Tick the simulation
    m_world_.update();
    
    // Package results into the triple buffer
    SimSnapshot& snap = m_sim_buffer_.get_write_buffer();

    // Filling the snapshot with information
    m_world_.fill_snapshot(snap);
	snap.stats.fps = fps_;
	snap.stats.m_total_time_elapsed_ = m_total_time_elapsed_;
    
    m_sim_buffer_.publish();


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

void Simulation::update_line_graphs(const SimSnapshot& snapshot)
{
    ++m_ticks_;

    if (snapshot.stats.iterations_ % 40 != 0)
        return;

    m_history_.push(
        snapshot.stats.iterations_,
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
    // Always grab the freshest completed simulation frame
    const SimSnapshot& snap = m_sim_buffer_.begin_read();
    float dt = static_cast<float>(m_delta_time_.get_delta());
    m_total_time_elapsed_ += dt;

    update_line_graphs(snap);
    handle_imGUI(snap, dt);

    m_sim_buffer_.end_read();

    m_window_.clear(GraphicalSettings::window_color);

    if (m_world_.toggles.m_rendering_)
    {
        const sf::Vector2f pos = camera_.get_world_mouse_pos();
        m_world_.render(&cell_statistic_font, pos);
    }

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

void Simulation::fill_snapshot(SimSnapshot& snapshot)
{
	m_world_.fill_snapshot(snapshot);
    snapshot.stats.m_total_time_elapsed_ = m_total_time_elapsed_;
	snapshot.stats.fps = fps_;
  
}