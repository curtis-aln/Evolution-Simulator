#include "../simulation.h"



void Simulation::imgui_population_graph()
{
    constexpr ImVec2 panel_pos = { 10.f, 800.f };
    constexpr ImVec2 panel_size = { 350.f, 160.f };
    constexpr ImVec2 plot_size = { -1.f, -1.f };
    constexpr float  scroll_window = 60.f;

    const float x_min = m_total_time_elapsed_ - scroll_window;
    const float x_max = m_total_time_elapsed_;
    const float y_max = static_cast<float>(std::max(WorldSettings::max_protozoa, FoodSettings::max_food));

    constexpr ImPlotFlags      plot_flags = ImPlotFlags_NoMenus | ImPlotFlags_NoBoxSelect | ImPlotFlags_NoMouseText;
    constexpr ImPlotAxisFlags  x_flags = ImPlotAxisFlags_None;
    constexpr ImPlotAxisFlags  y_flags = ImPlotAxisFlags_None;

    ImGui::SetNextWindowPos(panel_pos, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(panel_size, ImGuiCond_FirstUseEver);
    ImGui::Begin("Population", nullptr, ImGuiWindowFlags_NoTitleBar);

    // Top stats bar
    ImGui::Text("Protozoa: %d", m_world_.get_protozoa_count());
    ImGui::SameLine(0, 20);

    ImGui::Text("Food: %d", m_world_.get_food_count());
    ImGui::SameLine(0, 20);

    ImGui::Text("Avg Gen: %.2f", m_world_.average_generation_);
    ImGui::SameLine(0, 20);

    ImGui::Text("Frames: %d", m_ticks_);

    if (ImPlot::BeginPlot("##population", plot_size, plot_flags))
    {
        ImPlot::SetupAxes("Time (s)", "Count", x_flags, y_flags);
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
