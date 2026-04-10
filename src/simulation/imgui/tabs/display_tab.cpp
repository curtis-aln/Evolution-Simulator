#include "display_tab.h"
#include <imgui.h>

void DisplayTab::draw(UIContext& ctx)
{
    // ── 3 panels: Rendering | Debug Overlays | UI + Camera ────────────────────
    const float total = ImGui::GetContentRegionAvail().x;
    const float sp = ImGui::GetStyle().ItemSpacing.x;
    const float cw = (total - sp * 2.f) / 3.f;
    const float ch = -1.f;

    // ── Rendering ─────────────────────────────────────────────────────────────
    ImGui::BeginChild("DISP_rend", { cw, ch }, true);
    ImGui::TextDisabled("Rendering");
    ImGui::Separator();

    auto kb = [](const char* shortcut)
        {
            ImGui::SameLine();
            ImGui::TextDisabled("[%s]", shortcut);
        };

    ImGui::Checkbox("Enable Rendering", &ctx.rendering);               kb("R");
    ImGui::Checkbox("Simple Mode", &ctx.world.simple_mode);       kb("S");
    ImGui::Checkbox("Debug Mode", &ctx.world.debug_mode);        kb("D");
    ImGui::Checkbox("Cell Grid", &ctx.world.draw_cell_grid);    kb("G");
    ImGui::Checkbox("Food Grid", &ctx.world.draw_food_grid);    kb("F");
    ImGui::Checkbox("Track Statistics", &ctx.world.track_statistics);  kb("T");

    ImGui::EndChild();
    ImGui::SameLine();

    // ── Debug Overlays ────────────────────────────────────────────────────────
    ImGui::BeginChild("DISP_dbg", { cw, ch }, true);
    ImGui::TextDisabled("Debug Overlays  (debug mode only)");
    ImGui::Separator();

    ImGui::Checkbox("Show Connections", &ctx.world.show_connections);    kb("C");
    ImGui::Checkbox("Skeleton Mode", &ctx.world.skeleton_mode);       kb("K");
    ImGui::Checkbox("Bounding Boxes", &ctx.world.show_bounding_boxes); kb("B");
    ImGui::Checkbox("Toggle Collisions", &ctx.world.toggle_collisions);   kb("C");

    ImGui::EndChild();
    ImGui::SameLine();

    // ── UI + Camera ───────────────────────────────────────────────────────────
    ImGui::BeginChild("DISP_ui", { -1.f, ch }, true);
    ImGui::TextDisabled("UI");
    ImGui::Separator();

    ImGui::Checkbox("Hide Panels", &ctx.hide_panels);  kb("Q");

    ImGui::SetNextItemWidth(-1.f);
    if (ImGui::SliderFloat("##uiscale", &m_ui_scale_, 50.f, 200.f, "UI Scale %.0f%%"))
        ImGui::GetIO().FontGlobalScale = m_ui_scale_ / 100.f;

    ImGui::SetNextItemWidth(-1.f);
    ImGui::SliderFloat("##zoom", &m_zoom_slider_, 0.1f, 5.f, "Zoom %.2fx");
    ImGui::TextDisabled("TODO: wire to Camera::zoom()");

    ImGui::Spacing();
    ImGui::TextDisabled("Camera Lock");
    ImGui::Separator();

    const char* lock_opts[] = { "Free", "Lock to selected", "Lock to most successful" };
    ImGui::SetNextItemWidth(-1.f);
    ImGui::Combo("##camlock", &m_camera_lock_, lock_opts, 3);
    ImGui::TextDisabled("TODO: wire to camera_follow logic");

    ImGui::EndChild();
}