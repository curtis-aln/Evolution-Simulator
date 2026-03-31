#include "simulation.h"

void Simulation::init_imGUI()
{
    if (!ImGui::SFML::Init(m_window_))
        std::cerr << "[ERROR]: Failed to initialize ImGui-SFML\n";

    ImPlot::CreateContext();

    // define the two line colours: blue for protozoa, red for food
    static const ImVec4 plot_colors[2] = {
        {0.3f, 0.6f, 1.0f, 1.f},  // protozoa — blue
        {1.0f, 0.4f, 0.4f, 1.f},  // food      — red
    };
    m_plot_colormap_ = ImPlot::AddColormap("ARIAColors", plot_colors, 2, false);
}

void Simulation::imgui_stats_panel()
{
    constexpr ImVec2 panel_pos = { 10.f, 10.f };
    constexpr ImVec2 panel_size = { 260.f, 0.f };  // height 0 = auto-fit

    ImGui::SetNextWindowPos(panel_pos, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(panel_size, ImGuiCond_FirstUseEver);
    ImGui::Begin("Simulation Stats");

    ImGui::SeparatorText("Performance");
    ImGui::Text("FPS:             %.1f", fps_);
    ImGui::Text("Frames:          %d", m_ticks_);
    ImGui::Text("Time Elapsed:    %.2fs", m_total_time_elapsed_);
    ImGui::Text("Paused:          %s", m_world_.paused ? "Yes" : "No");

    ImGui::SeparatorText("Population");
    ImGui::Text("Protozoa:        %d", m_world_.get_protozoa_count());
    ImGui::Text("Food:            %d", m_world_.get_food_count());
    ImGui::Text("Avg Generation:  %.2f", m_world_.get_average_generation());
    ImGui::Text("Min Speed:       %.3f", m_world_.min_speed);
    ImGui::Text("Delta Min Speed: %.3f", m_world_.delta_min_speed);

    ImGui::End();
}

void Simulation::imgui_controls_panel()
{
    constexpr ImVec2 panel_pos = { 10.f, 300.f };
    constexpr ImVec2 panel_size = { 260.f, 0.f };

    ImGui::SetNextWindowPos(panel_pos, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(panel_size, ImGuiCond_FirstUseEver);
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
}

void Simulation::imgui_tuning_panel()
{
    constexpr ImVec2 panel_pos = { 10.f, 600.f };
    constexpr ImVec2 panel_size = { 260.f, 0.f };

    // delta_min_speed is stored at a very small scale (~0.001),
    // so we scale it up by 1000 for a usable slider range
    constexpr float delta_speed_scale = 1000.f;
    constexpr float delta_speed_slider_min = 0.2f;
    constexpr float delta_speed_slider_max = 2.0f;
    constexpr float min_speed_slider_min = 0.0f;
    constexpr float min_speed_slider_max = 135.0f;

    ImGui::SetNextWindowPos(panel_pos, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(panel_size, ImGuiCond_FirstUseEver);
    ImGui::Begin("Tuning");

    ImGui::SeparatorText("World");
    ImGui::SliderFloat("Min Speed", &m_world_.min_speed, min_speed_slider_min, min_speed_slider_max);

    // display in x1000 units, convert back on change
    float delta_speed_display = m_world_.delta_min_speed * delta_speed_scale;
    if (ImGui::SliderFloat("Delta Min Speed (x1000)", &delta_speed_display, delta_speed_slider_min, delta_speed_slider_max))
        m_world_.delta_min_speed = delta_speed_display / delta_speed_scale;

    ImGui::SeparatorText("Flags");
    ImGui::Checkbox("Rendering", &m_rendering_);
    ImGui::Checkbox("Paused", &m_world_.paused);
    ImGui::Checkbox("Debug Mode", &m_world_.debug_mode);
    ImGui::Checkbox("Simple Mode", &m_world_.simple_mode);

    ImGui::End();
}

void Simulation::imgui_population_graph()
{
    constexpr ImVec2 panel_pos = { 10.f, 800.f };
    constexpr ImVec2 panel_size = { 350.f, 160.f };
    constexpr ImVec2 plot_size = { -1.f, -1.f };   // fill available space
    constexpr float  scroll_window = 560.f;            // seconds of history shown

    const float x_min = m_total_time_elapsed_ - scroll_window;
    const float x_max = m_total_time_elapsed_;
    const float y_max = static_cast<float>(std::max(WorldSettings::max_protozoa, FoodSettings::max_food));

    constexpr ImPlotFlags  plot_flags = ImPlotFlags_NoMenus | ImPlotFlags_NoBoxSelect | ImPlotFlags_NoMouseText;
    constexpr ImPlotAxisFlags axis_flags = ImPlotAxisFlags_NoTickLabels;

    ImGui::SetNextWindowPos(panel_pos, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(panel_size, ImGuiCond_FirstUseEver);
    ImGui::Begin("Population", nullptr, ImGuiWindowFlags_NoTitleBar);

    if (ImPlot::BeginPlot("##population", plot_size, plot_flags))
    {
        ImPlot::SetupAxes(nullptr, nullptr, axis_flags, axis_flags);
        ImPlot::SetupAxisLimits(ImAxis_X1, x_min, x_max, ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, 0, y_max, ImGuiCond_Once);

        ImPlot::PushColormap(m_plot_colormap_);
        ImPlot::PlotLine("Protozoa", time_history_.data(), protozoa_history_.data(), static_cast<int>(protozoa_history_.size()));
        ImPlot::PlotLine("Food", time_history_.data(), food_history_.data(), static_cast<int>(food_history_.size()));
        ImPlot::PopColormap();

        ImPlot::EndPlot();
    }

    ImGui::End();
}

void Simulation::handle_imGUI()
{
    ImGui::SFML::Update(m_window_, m_delta_time_.get_delta_sfml());

    imgui_stats_panel();
    imgui_controls_panel();
    imgui_tuning_panel();
    imgui_population_graph();
}
