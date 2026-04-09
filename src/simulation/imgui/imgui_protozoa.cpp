#include "../simulation.h"



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
        ImGui::Text("Springs: %d", (int)selected_protozoa->get_springs().size());
        ImGui::Text("Offspring: %d", selected_protozoa->offspring_count);

        ImGui::Spacing();

        ImGui::Text("Energy: %.2f", selected_protozoa->get_energy());
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

        const auto& bounds = selected_protozoa->get_bounds();
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