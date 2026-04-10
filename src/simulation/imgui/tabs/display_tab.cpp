#include "display_tab.h"
#include <imgui.h>

void DisplayTab::draw(UIContext& ctx)
{
    ImGui::SeparatorText("Rendering");
    ImGui::Checkbox("Enable Rendering", &ctx.rendering);             ImGui::SameLine(); ImGui::TextDisabled("[R]");
    ImGui::Checkbox("Simple Mode", &ctx.world.simple_mode);     ImGui::SameLine(); ImGui::TextDisabled("[S]");
    ImGui::Checkbox("Debug Mode", &ctx.world.debug_mode);      ImGui::SameLine(); ImGui::TextDisabled("[D]");
    ImGui::Checkbox("Cell Grid", &ctx.world.draw_cell_grid);  ImGui::SameLine(); ImGui::TextDisabled("[G]");
    ImGui::Checkbox("Food Grid", &ctx.world.draw_food_grid);  ImGui::SameLine(); ImGui::TextDisabled("[F]");
    ImGui::Checkbox("Track Statistics", &ctx.world.track_statistics); ImGui::SameLine(); ImGui::TextDisabled("[T]");

    ImGui::Spacing();
    ImGui::SeparatorText("Debug Overlays (debug mode only)");
    ImGui::Checkbox("Show Connections", &ctx.world.show_connections);    ImGui::SameLine(); ImGui::TextDisabled("[C]");
    ImGui::Checkbox("Skeleton Mode", &ctx.world.skeleton_mode);       ImGui::SameLine(); ImGui::TextDisabled("[K]");
    ImGui::Checkbox("Bounding Boxes", &ctx.world.show_bounding_boxes); ImGui::SameLine(); ImGui::TextDisabled("[B]");
    ImGui::Checkbox("Toggle Collisions", &ctx.world.toggle_collisions);   ImGui::SameLine(); ImGui::TextDisabled("[C]");

    ImGui::Spacing();
    ImGui::SeparatorText("UI");
    ImGui::Checkbox("Hide Panels##disp", &ctx.hide_panels); ImGui::SameLine(); ImGui::TextDisabled("[Q]");

    ImGui::SetNextItemWidth(160.f);
    if (ImGui::SliderFloat("UI Scale##disp", &m_ui_scale_, 50.f, 200.f, "%.0f%%"))
        ImGui::GetIO().FontGlobalScale = m_ui_scale_ / 100.f;

    ImGui::SetNextItemWidth(160.f);
    ImGui::SliderFloat("Zoom (trackpad)##disp", &m_zoom_slider_, 0.1f, 5.f, "%.2fx");
    ImGui::TextDisabled("TODO: wire to Camera::zoom()");

    ImGui::Spacing();
    ImGui::SeparatorText("Camera Lock");
    const char* lock_opts[] = { "Free", "Lock to selected", "Lock to most successful" };
    ImGui::SetNextItemWidth(-1.f);
    ImGui::Combo("Mode##disp", &m_camera_lock_, lock_opts, 3);
    ImGui::TextDisabled("TODO: wire to Simulation::camera_follow logic");
}