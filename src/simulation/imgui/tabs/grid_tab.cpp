#include "grid_tab.h"
#include <imgui.h>

void GridTab::draw(UIContext& ctx)
{
    // ── 3 panels: Protozoa Grid | Food Grid | Tuning ──────────────────────────
    const float total = ImGui::GetContentRegionAvail().x;
    const float sp = ImGui::GetStyle().ItemSpacing.x;
    const float cw = (total - sp * 2.f) / 3.f;
    const float ch = -1.f;

    SimpleSpatialGrid* cg = ctx.world.get_spatial_grid();
    SimpleSpatialGrid* fg = ctx.world.get_food_spatial_grid();

    // ── Protozoa Grid ─────────────────────────────────────────────────────────
    ImGui::BeginChild("GR_cell", { cw, ch }, true);
    draw_grid_info("Protozoa Grid", *cg, ctx.tracking);
    ImGui::EndChild();
    ImGui::SameLine();

    // ── Food Grid ─────────────────────────────────────────────────────────────
    ImGui::BeginChild("GR_food", { cw, ch }, true);
    draw_grid_info("Food Grid", *fg, ctx.tracking);
    ImGui::EndChild();
    ImGui::SameLine();

    // ── Tuning ────────────────────────────────────────────────────────────────
    ImGui::BeginChild("GR_tune", { -1.f, ch }, true);
    ImGui::TextDisabled("Tuning");
    ImGui::Separator();

    ImGui::Checkbox("Track occupancy", &ctx.tracking);
    ImGui::SameLine();
    ImGui::TextDisabled("(costs perf)");

    ImGui::Spacing();
    static int res = static_cast<int>(cg->CellsX);
    ImGui::SetNextItemWidth(-1.f);
    ImGui::SliderInt("##gridres", &res, 10, 500, "Resolution %d x %d");

    ImGui::Spacing();
    if (ImGui::Button("Apply##grid", { -1.f, 0.f }))
    {
        cg->change_cell_dimsensions(res, res);
        fg->change_cell_dimsensions(res, res);
        ctx.world.update_spatial_renderers();
    }

    ImGui::EndChild();
}

void GridTab::draw_grid_info(const char* label, SimpleSpatialGrid& grid, bool tracking)
{
    ImGui::TextDisabled("%s", label);
    ImGui::Separator();

    ImGui::Text("%zux%zu  (%zu cells)", grid.CellsX, grid.CellsY, grid.get_total_cells());
    ImGui::Text("Cell  %.1f x %.1f px", grid.cell_width, grid.cell_height);
    ImGui::Text("World %.0f x %.0f", grid.world_width, grid.world_height);
    ImGui::Text("Cap   %zu obj/cell", grid.cell_max_capacity);

    if (!tracking)
    {
        ImGui::Spacing();
        ImGui::TextDisabled("(tracking disabled)");
        return;
    }

    int total = 0, max_in = 0, full = 0, empty = 0;
    for (size_t i = 0; i < grid.get_total_cells(); ++i)
    {
        const int c = static_cast<int>(grid.cell_capacities[i]);
        total += c;
        if (c == 0)                                       ++empty;
        if (c >= static_cast<int>(grid.cell_max_capacity)) ++full;
        if (c > max_in)                                    max_in = c;
    }

    const size_t tc = grid.get_total_cells();
    const float  inv = tc > 0 ? 1.f / (float)tc : 0.f;

    ImGui::Spacing();
    ImGui::Text("Objects  %d", total);
    ImGui::Text("Avg/cell %.2f", tc > 0 ? (float)total * inv : 0.f);
    ImGui::Text("Max cell %d  (%.0f%%)", max_in,
        grid.cell_max_capacity > 0 ? max_in * 100.f / grid.cell_max_capacity : 0.f);
    ImGui::Text("Full     %d  (%.1f%%)", full, full * 100.f * inv);
    ImGui::Text("Empty    %d  (%.1f%%)", empty, empty * 100.f * inv);

    if (full > 0)
        ImGui::TextColored({ 1.f, 0.4f, 0.4f, 1.f },
            "[!] %d at cap — objects may drop", full);
}