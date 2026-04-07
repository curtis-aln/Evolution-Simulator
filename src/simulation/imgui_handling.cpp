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

    ImGuiStyle& style = ImGui::GetStyle();

    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0, 0, 0, 0.7f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);

    // in init_imGUI, after ImGui::SFML::Init
    constexpr float ui_scale_percent = 250.f;
    ImGui::GetIO().FontGlobalScale = ui_scale_percent / 100.f;
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

    ImGui::SeparatorText("World");
    m_world_.frames_per_generation_ == -1
        ? ImGui::Text("Frames per Gen: insufficient data")
        : ImGui::Text("Frames per Gen: ", m_world_.frames_per_generation_);

    ImGui::Text("Avg Cells per Proto: %.1f", m_world_.average_cells_per_protozoa_);
    ImGui::Text("Avg Offspring: %.1f", m_world_.average_offspring_count_);
    ImGui::Text("Avg Mut Rate: %.4f", m_world_.average_mutation_rate_);
    ImGui::Text("Avg Mut Range: %.4f", m_world_.average_mutation_range_);

    ImGui::Text("Avg Lifetime: %.2f", m_world_.average_lifetime_);
    ImGui::Text("Births /100f: %.2f", m_world_.births_per_hundered_frames_);
    ImGui::Text("Deaths /100f: %.2f", m_world_.deaths_per_hundered_frames_);

    ImGui::End();
}

void Simulation::imgui_controls_panel()
{
    constexpr ImVec2 panel_pos = { 10.f, 300.f };
    constexpr ImVec2 panel_size = { 260.f, 0.f };

    ImGui::SetNextWindowPos(panel_pos, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(panel_size, ImGuiCond_FirstUseEver);
    ImGui::Begin("Controls");

    // ------------------ Simulation ------------------
    ImGui::SeparatorText("Simulation");

    ImGui::Text("Step Frame");
    ImGui::SameLine();
    ImGui::TextDisabled("[O]");

    if (ImGui::Button("Reset Simulation"))
    {
        open_extinction_window = true;
    }

    // ------------------ Camera ------------------
    ImGui::SeparatorText("Camera");

    ImGui::Text("Zoom");
    ImGui::SameLine();
    ImGui::TextDisabled("[Scroll]");

    ImGui::Text("Pan");
    ImGui::SameLine();
    ImGui::TextDisabled("[LMB Hold]");

    // ------------------ Exit ------------------
    ImGui::Separator();

    if (ImGui::Button("Exit Program [ESC]", { -1, 0 }))
    {
		running = false;
    }
    ImGui::SameLine();
    ImGui::TextDisabled("[Esc]");

    ImGui::End();
}


void Simulation::imgui_tuning_panel()
{
    constexpr ImVec2 panel_pos = { 10.f, 600.f };
    constexpr ImVec2 panel_size = { 260.f, 0.f };

    constexpr float delta_speed_scale = 1000.f;
    constexpr float delta_speed_slider_min = 0.2f;
    constexpr float delta_speed_slider_max = 2.0f;
    constexpr float min_speed_slider_min = 0.0f;
    constexpr float min_speed_slider_max = 135.0f;

    ImGui::SetNextWindowPos(panel_pos, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(panel_size, ImGuiCond_FirstUseEver);
    ImGui::Begin("Tuning");

    // ------------------ World ------------------
    ImGui::SeparatorText("World");

    ImGui::SliderFloat("Min Speed", &m_world_.min_speed, min_speed_slider_min, min_speed_slider_max);

    float delta_speed_display = m_world_.delta_min_speed * delta_speed_scale;
    if (ImGui::SliderFloat("Delta Min Speed (x1000)", &delta_speed_display, delta_speed_slider_min, delta_speed_slider_max))
        m_world_.delta_min_speed = delta_speed_display / delta_speed_scale;

    // ------------------ Simulation + Display (side-by-side) ------------------
    ImGui::SeparatorText("Flags");

    float full_width = ImGui::GetContentRegionAvail().x;
    float column_width = full_width * 0.5f;

    // LEFT COLUMN (Simulation)
    ImGui::BeginChild("SimFlags", ImVec2(column_width, 0), false);

    ImGui::TextDisabled("Simulation");

    ImGui::Checkbox("Rendering", &m_rendering_);
    ImGui::SameLine(); ImGui::TextDisabled("[R]");

    ImGui::Checkbox("Paused", &m_world_.paused);
    ImGui::SameLine(); ImGui::TextDisabled("[Space]");

    ImGui::Checkbox("Collisions", &m_world_.toggle_collisions);
    ImGui::SameLine(); ImGui::TextDisabled("[C]");

    ImGui::Checkbox("Debug", &m_world_.debug_mode);
    ImGui::SameLine(); ImGui::TextDisabled("[D]");

   
    if (ImGui::Button("Step"))
    {
        m_tick_frame_time = true;
        m_world_.paused = true;
    }
    ImGui::SameLine();
    ImGui::TextDisabled("[O]");
   

    ImGui::EndChild();

    ImGui::SameLine();

    // RIGHT COLUMN (Display)
    ImGui::BeginChild("DisplayFlags", ImVec2(0, 0), false);

    ImGui::TextDisabled("Display");

    ImGui::Checkbox("Cell Grid", &m_world_.draw_cell_grid);
    ImGui::SameLine(); ImGui::TextDisabled("[G]");

    ImGui::Checkbox("Food Grid", &m_world_.draw_food_grid);
    ImGui::SameLine(); ImGui::TextDisabled("[F]");

    ImGui::Checkbox("Simple", &m_world_.simple_mode);
    ImGui::SameLine(); ImGui::TextDisabled("[S]");

    ImGui::Checkbox("Track Stats", &m_world_.track_statistics);
    ImGui::SameLine(); ImGui::TextDisabled("[T]");

    ImGui::Checkbox("Hide Panels", &hide_panels);
    ImGui::SameLine(); ImGui::TextDisabled("[Q]");

    ImGui::EndChild();

    ImGui::End();
}

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

void Simulation::handle_imGUI()
{
    ImGui::SFML::Update(m_window_, m_delta_time_.get_delta_sfml());

    imgui_stats_panel();
    imgui_controls_panel();
    imgui_tuning_panel();
    imgui_population_graph();

    if (m_world_.selected_protozoa_ && m_world_.debug_mode)
		imgui_debug_panel(nullptr, m_world_.selected_protozoa_);

    if (open_extinction_window)
    {
        ImGui::OpenPopup("New Simulation");
        open_extinction_window = false;
    }
    extinction_popup();

    imgui_spatial_grid_panel();
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



void Simulation::imgui_debug_panel(Cell* selected_cell, Protozoa* selected_protozoa)
{
    if (!selected_cell && !selected_protozoa)
        return;

    ImGui::Begin("Debug Inspector");

    float full_width = ImGui::GetContentRegionAvail().x;
    float column_width = full_width * 0.4f;

    // ------------------ CELL ------------------
    if (selected_cell)
    {
        ImGui::SeparatorText("Cell");

        ImGui::Text("ID: %d", selected_cell->id);
        ImGui::Text("Generation: %d", selected_cell->generation);
        ImGui::Text("Friction: %.3f", selected_cell->sinwave_current_friction_);

        ImGui::Spacing();

        ImGui::Text("Position: (%.2f, %.2f)", selected_cell->position_.x, selected_cell->position_.y);
        ImGui::Text("Velocity: (%.2f, %.2f)", selected_cell->velocity_.x, selected_cell->velocity_.y);
        ImGui::Text("Speed: %.2f", selected_cell->velocity_.length());

        ImGui::Spacing();

        ImGui::Text("Radius: %.2f", selected_cell->radius);
    }

    // ------------------ PROTOZOA (TWO COLUMNS) ------------------
    if (selected_protozoa)
    {
        ImGui::SeparatorText("Protozoa");

        // LEFT COLUMN
        ImGui::BeginChild("Protozoa_Left", ImVec2(column_width, 0), false);

        ImGui::Text("ID: %d", selected_protozoa->id);
        // ImGui::Text("Generation: %d", selected_protozoa->generation); // todo
        ImGui::Text("Age: %d", selected_protozoa->frames_alive);

        ImGui::Spacing();

        ImGui::Text("Cells: %d", (int)selected_protozoa->get_cells().size());
        ImGui::Text("Springs: %d", (int)selected_protozoa->m_springs_.size());
        ImGui::Text("Offspring: %d", selected_protozoa->offspring_count);

        ImGui::Spacing();

        ImGui::Text("Energy: %.2f", selected_protozoa->energy);
        ImGui::Text("Food: %.2f", selected_protozoa->total_food_eaten);
		ImGui::Text("Energy lost to springs: %.2f", selected_protozoa->energy_lost_to_springs);
        ImGui::TextDisabled("Display");

        ImGui::Checkbox("Skeleton", &m_world_.skeleton_mode);
        ImGui::SameLine(); ImGui::TextDisabled("[K]");

        ImGui::Checkbox("Bounds", &m_world_.show_bounding_boxes);
        ImGui::SameLine(); ImGui::TextDisabled("[B]");

        ImGui::EndChild();

        ImGui::SameLine();

        // RIGHT COLUMN
        ImGui::BeginChild("Protozoa_Right", ImVec2(0, 0), false);

        ImGui::TextDisabled("Mutation");

        //ImGui::Text("Range: %.4f", selected_protozoa->mutation_range);
        //ImGui::Text("Rate: %.4f", selected_protozoa->mutation_rate); // todo

        ImGui::Spacing();

        ImGui::TextDisabled("Transform");

        ImGui::Text("Position: (%.2f, %.2f)",
            selected_protozoa->get_center().x,
            selected_protozoa->get_center().y);

        const auto& bounds = selected_protozoa->m_personal_bounds_;
        ImGui::Text("Bounds: (%.0f, %.0f)",
            bounds.size.x, bounds.size.y);
        ImGui::Text("Area: %.0f", bounds.size.x * bounds.size.y);

        ImGui::Text("Velocity: (%.2f, %.2f)",
            selected_protozoa->velocity.x,
            selected_protozoa->velocity.y);

        ImGui::Text("Speed: %.1f", selected_protozoa->velocity.length());

        const sf::Vector2f pos = selected_protozoa->get_center();
        const float dist_to_birth_loc = std::sqrt(std::pow(pos.x - selected_protozoa->birth_location.x, 2) +
			std::pow(pos.y - selected_protozoa->birth_location.y, 2));

		ImGui::Text("Dist to Birth Loc: %.0f", dist_to_birth_loc);

        ImGui::Spacing();

        ImGui::EndChild();
    }

    ImGui::End();
}

void Simulation::imgui_spatial_grid_panel()
{
    ImGui::Begin("Spatial Grid Inspector");

    ImGui::Checkbox("Track", &tracking);
    ImGui::SameLine();
    ImGui::TextDisabled("(disable to save performance)");

    SimpleSpatialGrid* cell_grid = m_world_.get_spatial_grid();
    SimpleSpatialGrid* food_grid = m_world_.get_food_spatial_grid();

    auto draw_grid_info = [this](const char* label, SimpleSpatialGrid& grid)
        {
            ImGui::SeparatorText(label);

            ImGui::Text("Grid:        %zu x %zu  (%zu cells)", grid.CellsX, grid.CellsY, grid.get_total_cells());
            ImGui::Text("Cell Size:   %.1f x %.1f px", grid.cell_width, grid.cell_height);
            ImGui::Text("World Size:  %.0f x %.0f", grid.world_width, grid.world_height);
            ImGui::Text("Cell Cap:    %zu obj/cell", grid.cell_max_capacity);

            if (!tracking)
            {
                ImGui::TextDisabled("(tracking disabled)");
                return;
            }

            int total_objects = 0;
            int max_in_cell = 0;
            int full_cells = 0;
            int empty_cells = 0;

            for (size_t i = 0; i < grid.get_total_cells(); ++i)
            {
                const int count = static_cast<int>(grid.cell_capacities[i]);
                total_objects += count;
                if (count == 0) ++empty_cells;
                if (count >= static_cast<int>(grid.cell_max_capacity)) ++full_cells;
                if (count > max_in_cell) max_in_cell = count;
            }

            const size_t total_cells = grid.get_total_cells();

            const float avg = total_cells > 0
                ? static_cast<float>(total_objects) / static_cast<float>(total_cells)
                : 0.f;
            const float fill_pct = grid.cell_max_capacity > 0
                ? static_cast<float>(max_in_cell) / static_cast<float>(grid.cell_max_capacity) * 100.f
                : 0.f;
            const float empty_pct = total_cells > 0
                ? static_cast<float>(empty_cells) / static_cast<float>(total_cells) * 100.f
                : 0.f;
            const float full_pct = total_cells > 0
                ? static_cast<float>(full_cells) / static_cast<float>(total_cells) * 100.f
                : 0.f;

            ImGui::Spacing();
            ImGui::Text("Total Objects:  %d", total_objects);
            ImGui::Text("Avg per Cell:   %.2f", avg);
            ImGui::Text("Max in Cell:    %d  (%.0f%% full)", max_in_cell, fill_pct);
            ImGui::Text("Full Cells:     %d  (%.1f%% of grid)", full_cells, full_pct);
            ImGui::Text("Empty Cells:    %d  (%.1f%% of grid)", empty_cells, empty_pct);

            if (full_cells > 0)
                ImGui::TextColored({ 1.f, 0.4f, 0.4f, 1.f }, "[!] %d cell(s) at capacity — objects may be dropped", full_cells);
        };

    draw_grid_info("Protozoa Grid", *cell_grid);
    ImGui::Spacing();
    draw_grid_info("Food Grid", *food_grid);

    // ------------------ Tuning ------------------
    ImGui::Spacing();
    ImGui::SeparatorText("Tuning");

    static int grid_resolution = static_cast<int>(cell_grid->CellsX);
    ImGui::SliderInt("Grid Resolution (N x N)", &grid_resolution, 10, 500);

    static float world_size = cell_grid->world_width;
    ImGui::InputFloat("World Size", &world_size, 1000.f, 10000.f, "%.0f");

    if (ImGui::Button("Apply"))
    {
        cell_grid->update_cell_dimensions();
        food_grid->update_cell_dimensions();
    }

    ImGui::End();
}