#pragma once

#include <cstdint>
#include <cstring>
#include <cassert>
#include <algorithm>
#include <vector>
#include <Imgui.h>
#include "../spatial_grid/simple_spatial_grid.h" // calcZOrder, mortonToX/Y, mortonNeighbours3x3, isPow2, cell_idx — adjust path if named differently

struct PheromoneGridSettings
{
    float decay_rate = 0.02f;  // fraction of pheromone lost per step
    float diffuse_rate = 0.15f;  // blend toward 3x3 neighbour average per step
    float deposit_amount = 10.0f;  // default amount added per add_pheromone call
    float max_pheromone = 100.0f; // clamp ceiling, also used to normalise heatmap colour
};

class PheromoneGrid
{
public:
    explicit PheromoneGrid(uint32_t cells_x, uint32_t cells_y,
        float world_width, float world_height,
        PheromoneGridSettings settings = {})
        : CellsX(cells_x), CellsY(cells_y)
        , world_width(world_width), world_height(world_height)
        , settings(settings)
    {
        // Same constraint as SimpleSpatialGrid — Morton indexing needs power-of-2 dims.
        assert(isPow2(cells_x) && "CellsX must be a power of 2 for Morton indexing");
        assert(isPow2(cells_y) && "CellsY must be a power of 2 for Morton indexing");

        const uint32_t total = mortonTableSize(cells_x, cells_y);
        front.assign(total, 0.f);
        back.assign(total, 0.f);

        update_cell_dimensions();
    }

    void update_cell_dimensions()
    {
        cell_width = world_width / static_cast<float>(CellsX);
        cell_height = world_height / static_cast<float>(CellsY);
    }

    void change_cell_dimensions(uint32_t new_cells_x, uint32_t new_cells_y)
    {
        assert(isPow2(new_cells_x) && isPow2(new_cells_y));
        CellsX = new_cells_x;
        CellsY = new_cells_y;
        update_cell_dimensions();

        const uint32_t total = mortonTableSize(CellsX, CellsY);
        front.assign(total, 0.f);
        back.assign(total, 0.f);
    }

    cell_idx inline hash(const float x, const float y) const
    {
        const auto cell_x = static_cast<uint16_t>(x / cell_width);
        const auto cell_y = static_cast<uint16_t>(y / cell_height);
        return calcZOrder(cell_x, cell_y);
    }

    void add_pheromone(const float x, const float y, float amount = -1.f)
    {
        add_pheromone_at_index(hash(x, y), amount);
    }

    void add_pheromone_at_index(const cell_idx index, float amount = -1.f)
    {
        if (amount < 0.f) amount = settings.deposit_amount;
        float& v = front[index];
        v = std::min(v + amount, settings.max_pheromone);
    }

    float sample(const float x, const float y) const
    {
        return front[hash(x, y)];
    }

    float sample_at_index(const cell_idx index) const
    {
        return front[index];
    }

    void clear()
    {
        std::memset(front.data(), 0, front.size() * sizeof(float));
        std::memset(back.data(), 0, back.size() * sizeof(float));
    }

    // Diffuses toward the 3x3 neighbour average, then decays. Call once per sim tick.
    void step()
    {
        for (uint32_t cy = 0; cy < CellsY; ++cy)
        {
            for (uint32_t cx = 0; cx < CellsX; ++cx)
            {
                const cell_idx idx = calcZOrder(static_cast<uint16_t>(cx), static_cast<uint16_t>(cy));

                uint32_t neighbours[9];
                mortonNeighbours3x3(idx, neighbours);

                float sum = 0.f;
                int   valid = 0;

                // Same edge handling as SimpleSpatialGrid::find_from_index — decode
                // and bounds-check rather than skip via signed arithmetic.
                for (int i = 0; i < 9; ++i)
                {
                    const uint32_t nx = mortonToX(neighbours[i]);
                    const uint32_t ny = mortonToY(neighbours[i]);
                    if (nx >= CellsX || ny >= CellsY) continue;

                    sum += front[neighbours[i]];
                    ++valid;
                }

                const float avg = valid > 0 ? sum / static_cast<float>(valid) : front[idx];
                float v = front[idx] + (avg - front[idx]) * settings.diffuse_rate;
                v *= (1.f - settings.decay_rate);

                back[idx] = v < 0.01f ? 0.f : v; // floor tiny residuals to zero
            }
        }

        std::swap(front, back);
    }

    size_t get_total_cells() const { return CellsX * CellsY; }

    // Heatmap renderer — call inside an ImGui window with an active draw list.
    // origin is the top-left screen position of the grid; px_per_unit converts world -> screen.
    void render(ImDrawList* draw_list, ImVec2 origin, float px_per_unit) const
    {
        for (uint32_t cy = 0; cy < CellsY; ++cy)
        {
            for (uint32_t cx = 0; cx < CellsX; ++cx)
            {
                const cell_idx idx = calcZOrder(static_cast<uint16_t>(cx), static_cast<uint16_t>(cy));
                const float v = front[idx];
                if (v <= 0.f) continue;

                const ImVec2 p0(origin.x + cx * cell_width * px_per_unit,
                    origin.y + cy * cell_height * px_per_unit);
                const ImVec2 p1(p0.x + cell_width * px_per_unit,
                    p0.y + cell_height * px_per_unit);

                draw_list->AddRectFilled(p0, p1, heatmap_color(v));
            }
        }
    }

    void track_stats()
    {
        float total = 0.f, max_v = 0.f;
        int active = 0;

        for (uint32_t cy = 0; cy < CellsY; ++cy)
        {
            for (uint32_t cx = 0; cx < CellsX; ++cx)
            {
                const float v = front[calcZOrder(static_cast<uint16_t>(cx), static_cast<uint16_t>(cy))];
                total += v;
                if (v > max_v) max_v = v;
                if (v > 0.f)   ++active;
            }
        }

        const float tc = static_cast<float>(get_total_cells());

        ImGui::Spacing();
        ImGui::Text("Total     %.1f", total);
        ImGui::Text("Avg/cell  %.3f", tc > 0.f ? total / tc : 0.f);
        ImGui::Text("Max cell  %.2f  (%.0f%%)", max_v,
            settings.max_pheromone > 0.f ? max_v * 100.f / settings.max_pheromone : 0.f);
        ImGui::Text("Active    %d  (%.1f%%)", active, tc > 0.f ? active * 100.f / tc : 0.f);
    }

    PheromoneGridSettings settings;

private:
    // Blue -> cyan -> yellow -> red ramp, alpha scaling with intensity so empty cells stay invisible.
    ImU32 heatmap_color(float v) const
    {
        const float t = std::clamp(v / settings.max_pheromone, 0.f, 1.f);
        float r, g, b;
        if (t < 0.5f) { r = 0.f;              g = t * 2.f;              b = 1.f - t * 2.f; }
        else { r = (t - 0.5f) * 2.f; g = 1.f - (t - 0.5f) * 2.f; b = 0.f; }
        return ImGui::ColorConvertFloat4ToU32(ImVec4(r, g, b, 0.15f + t * 0.6f));
    }

    // Same rationale as SimpleSpatialGrid::mortonTableSize — allocate to the highest
    // Morton index actually reachable, not CellsX*CellsY.
    static uint32_t mortonTableSize(uint32_t cx, uint32_t cy)
    {
        return calcZOrder(static_cast<uint16_t>(cx - 1),
            static_cast<uint16_t>(cy - 1)) + 1;
    }

    uint32_t CellsX = 0;
    uint32_t CellsY = 0;

    float cell_width = 0;
    float cell_height = 0;
    float world_width = 0;
    float world_height = 0;

    alignas(64) std::vector<float> front{};
    alignas(64) std::vector<float> back{};
};