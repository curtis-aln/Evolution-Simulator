#include "../simulation.h"

void Simulation::init_imGUI()
{
    if (!ImGui::SFML::Init(m_window_))
        std::cerr << "[ERROR]: Failed to initialize ImGui-SFML\n";

    ImGui::GetIO().FontGlobalScale = ui_scale_percent / 100.f;

    ImPlot::CreateContext();
}

void Simulation::handle_imGUI()
{
    ImGui::SFML::Update(m_window_, m_delta_time_.get_delta_sfml());

    UIContext ctx
    {
        .world = m_world_,
        .history = m_history_,
        .total_time_elapsed = m_total_time_elapsed_,
        .ticks = m_ticks_,
        .fps = fps_,
        .rendering = m_rendering_,
        .hide_panels = hide_panels,
        .open_extinction_window = open_extinction_window,
        .tick_frame_time = m_tick_frame_time,
        .running = running,
        .tracking = tracking,
        .plot_colormap = m_plot_colormap_,
    };

    const float dt = static_cast<float>(m_delta_time_.get_delta());
    m_control_panel_.draw(ctx, dt);

    if (open_extinction_window)
    {
        ImGui::OpenPopup("New Simulation");
        open_extinction_window = false;
    }
}

void Simulation::extinction_popup()
{
    if (ImGui::BeginPopupModal("New Simulation", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        // Center the popup
        ImGui::SetWindowPos(
            ImVec2(
                (ImGui::GetIO().DisplaySize.x - ImGui::GetWindowSize().x) * 0.5f,
                (ImGui::GetIO().DisplaySize.y - ImGui::GetWindowSize().y) * 0.5f
            )
        );

        static int starting_food = 1000;
        static int starting_protozoa = 50;
        static int max_food = 2000;
        static int max_protozoa = 200;
        static float world_radius = 10'000;

        static float mutation_rate = 0.01f;
        static float mutation_range = 0.1f;
        static float food_spawn_rate = 0.5f;

        ImGui::SeparatorText("Simulation Settings");

        int step_1 = 100;
        int step_fast = 1000;
        ImGui::InputInt("Starting Food", &starting_food, step_1, step_fast);
        ImGui::InputInt("Starting Protozoa", &starting_protozoa, step_1, step_fast);
        ImGui::InputInt("Max Food", &max_food, step_1, step_fast);
        ImGui::InputInt("Max Protozoa", &max_protozoa, step_1, step_fast);

        step_1 = 1000;
        step_fast = 10'000;
        ImGui::InputFloat("World radius", &world_radius, step_1, step_fast);
#
        ImGui::Spacing();

        ImGui::SeparatorText("Evolution Settings");

        ImGui::InputFloat("Mutation Rate", &mutation_rate, 0.001f, 0.01f, "%.4f");
        ImGui::InputFloat("Mutation Range", &mutation_range, 0.01f, 0.1f, "%.4f");
        ImGui::InputFloat("Food Spawn Rate", &food_spawn_rate, 0.01f, 1.0f, "%.2f");

        ImGui::Spacing();

        if (ImGui::Button("Start"))
        {

            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}