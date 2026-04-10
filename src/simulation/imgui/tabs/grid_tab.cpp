#include "grid_tab.h"
#include <imgui.h>

void GridTab::draw(UIContext& ctx)
{
    ImGui::Checkbox("Track occupancy##grid", &ctx.tracking);
    ImGui::SameLine(); ImGui::TextDisabled("(disable to save performance)");

    draw_grid_info("Protozoa Grid", *ctx.world.get_spatial_grid(), ctx.tracking);
    ImGui::Spacing();
    draw_grid_info("Food Grid", *ctx.world.get_food_spatial_grid(), ctx.tracking);
    ImGui::Spacing();
    draw_tuning(ctx);
}

void GridTab::draw_grid_info(const char* label, SimpleSpatialGrid& grid, bool tracking)
{
    ImGui::SeparatorText(label);
    ImGui::Text("Grid:       %zu x %zu  (%zu cells)", grid.CellsX, grid.CellsY, grid.get_total_cells());
    ImGui::Text("Cell size:  %.1f x %.1f px", grid.cell_width, grid.cell_height);
    ImGui::Text("World size: %.0f x %.0f", grid.world_width, grid.world_height);
    ImGui::Text("Cell cap:   %zu obj/cell", grid.cell_max_capacity);

    if (!tracking) { ImGui::TextDisabled("(tracking disabled)"); return; }

    int total = 0, max_in = 0, full = 0, empty = 0;
    for (size_t i = 0; i < grid.get_total_cells(); ++i)
    {
        const int c = static_cast<int>(grid.cell_capacities[i]);
        total += c;
        if (c == 0) ++empty;
        if (c >= static_cast<int>(grid.cell_max_capacity)) ++full;
        if (c > max_in) max_in = c;
    }
    const size_t tc = grid.get_total_cells();
    ImGui::Spacing();
    ImGui::Text("Total objects:  %d", total);
    ImGui::Text("Avg per cell:   %.2f", tc > 0 ? (float)total / tc : 0.f);
    ImGui::Text("Max in cell:    %d  (%.0f%% cap)", max_in, grid.cell_max_capacity > 0 ? max_in * 100.f / grid.cell_max_capacity : 0.f);
    ImGui::Text("Full cells:     %d  (%.1f%% of grid)", full, tc > 0 ? full * 100.f / tc : 0.f);
    ImGui::Text("Empty cells:    %d  (%.1f%% of grid)", empty, tc > 0 ? empty * 100.f / tc : 0.f);

    if (full > 0)
        ImGui::TextColored({ 1.f, 0.4f, 0.4f, 1.f },
            "[!] %d cell(s) at capacity — objects may be dropped", full);
}

void GridTab::draw_tuning(UIContext& ctx)
{
    ImGui::SeparatorText("Tuning");
    SimpleSpatialGrid* cg = ctx.world.get_spatial_grid();
    SimpleSpatialGrid* fg = ctx.world.get_food_spatial_grid();

    static int res = static_cast<int>(cg->CellsX);
    ImGui::SetNextItemWidth(-1.f);
    ImGui::SliderInt("Resolution (N x N)##grid", &res, 10, 500);

    if (ImGui::Button("Apply##grid", { -1.f, 0.f }))
    {
        cg->change_cell_dimsensions(res, res);
        fg->change_cell_dimsensions(res, res);
        ctx.world.update_spatial_renderers();
    }
}