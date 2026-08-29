#pragma once

#include <cstdint>
#include <cstring>
#include <cassert>
#include <algorithm>
#include <vector>
#include <Imgui.h>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>

struct PheromoneGridSettings
{
    inline static float decay_rate = 0.0015f;  // fraction of pheromone lost per step
    inline static float diffuse_rate = 0.20f;  // blend toward weighted neighbour average per step
    inline static float deposit_amount = 10.0f;  // default amount added per add_pheromone call
    inline static float max_pheromone = 100.0f; // clamp ceiling, also used to normalise heatmap colour
    inline static uint32_t substeps = 2;      // diffusion sub-iterations per step() call — higher = smoother spread, costs more per tick
};

// Dense full-grid diffusion, not a sparse query structure — plain row-major storage with a
// 1-cell clamp-to-edge border beats Morton indexing here: no per-cell encode/decode, a fully
// separable 1-2-1 kernel (two 3-tap passes instead of one 9-tap 2D kernel), and linear memory
// access the compiler can auto-vectorize. Rendering uploads the grid as a texture and draws
// one sprite instead of rebuilding a per-cell triangle mesh every frame.
class PheromoneGrid : PheromoneGridSettings
{
public:
    explicit PheromoneGrid(uint32_t cells_x, uint32_t cells_y,
        float world_width, float world_height)
        : CellsX(cells_x), CellsY(cells_y)
        , PaddedW(cells_x + 2), PaddedH(cells_y + 2)
        , world_width(world_width), world_height(world_height)
    {
        assert(cells_x > 0 && cells_y > 0);

        front.assign(static_cast<size_t>(PaddedW) * PaddedH, 0.f);
        back.assign(static_cast<size_t>(PaddedW) * PaddedH, 0.f);
        h_pass.assign(static_cast<size_t>(CellsX) * PaddedH, 0.f);

        update_cell_dimensions();
        resize_render_texture();
    }

    void update_cell_dimensions()
    {
        cell_width = world_width / static_cast<float>(CellsX);
        cell_height = world_height / static_cast<float>(CellsY);
    }

    void change_cell_dimensions(uint32_t new_cells_x, uint32_t new_cells_y)
    {
        assert(new_cells_x > 0 && new_cells_y > 0);
        CellsX = new_cells_x;
        CellsY = new_cells_y;
        PaddedW = CellsX + 2;
        PaddedH = CellsY + 2;
        update_cell_dimensions();

        front.assign(static_cast<size_t>(PaddedW) * PaddedH, 0.f);
        back.assign(static_cast<size_t>(PaddedW) * PaddedH, 0.f);
        h_pass.assign(static_cast<size_t>(CellsX) * PaddedH, 0.f);

        resize_render_texture();
    }

    uint32_t hash(const float x, const float y) const
    {
        const uint32_t cx = std::min(static_cast<uint32_t>(x / cell_width), CellsX - 1);
        const uint32_t cy = std::min(static_cast<uint32_t>(y / cell_height), CellsY - 1);
        return (cy + 1) * PaddedW + (cx + 1);
    }

    void add_pheromone(const float x, const float y, float amount = -1.f)
    {
        add_pheromone_at_index(hash(x, y), amount);
    }

    void add_pheromone_at_index(const uint32_t index, float amount = -1.f)
    {
        if (amount < 0.f) amount = deposit_amount;
        float& v = front[index];
        v = std::min(v + amount, max_pheromone);
    }

    float sample(const float x, const float y) const
    {
        return front[hash(x, y)];
    }

    float sample_at_index(const uint32_t index) const
    {
        return front[index];
    }

    void clear()
    {
        std::memset(front.data(), 0, front.size() * sizeof(float));
        std::memset(back.data(), 0, back.size() * sizeof(float));
    }

    // Diffuses via a separable 1-2-1 kernel (equivalent to the 1-2-1/2-4-2/1-2-1 2D kernel
    // at roughly a third of the reads), then decays. Call once per sim tick.
    void step()
    {
        const uint32_t n = std::max(1u, substeps);
        const float step_diffuse = diffuse_rate / static_cast<float>(n);
        const float step_decay = decay_rate / static_cast<float>(n);

        for (uint32_t s = 0; s < n; ++s)
        {
            replicate_border();

            // Horizontal pass: front (padded) -> h_pass (interior columns, all padded rows)
            for (uint32_t y = 0; y < PaddedH; ++y)
            {
                const float* __restrict row = &front[static_cast<size_t>(y) * PaddedW];
                float* __restrict out = &h_pass[static_cast<size_t>(y) * CellsX];

                for (uint32_t x = 0; x < CellsX; ++x)
                    out[x] = (row[x] + 2.f * row[x + 1] + row[x + 2]) * 0.25f;
            }

            // Vertical pass + blend + decay: h_pass -> back (padded interior)
            for (uint32_t y = 0; y < CellsY; ++y)
            {
                const float* __restrict h_top = &h_pass[static_cast<size_t>(y) * CellsX];
                const float* __restrict h_mid = &h_pass[static_cast<size_t>(y + 1) * CellsX];
                const float* __restrict h_bot = &h_pass[static_cast<size_t>(y + 2) * CellsX];
                const float* __restrict orig = &front[static_cast<size_t>(y + 1) * PaddedW + 1];
                float* __restrict out = &back[static_cast<size_t>(y + 1) * PaddedW + 1];

                for (uint32_t x = 0; x < CellsX; ++x)
                {
                    const float avg = (h_top[x] + 2.f * h_mid[x] + h_bot[x]) * 0.25f;
                    float v = orig[x] + (avg - orig[x]) * step_diffuse;
                    v *= (1.f - step_decay);
                    out[x] = v < 0.01f ? 0.f : v;
                }
            }

            std::swap(front, back);
        }
    }

    size_t get_total_cells() const { return static_cast<size_t>(CellsX) * CellsY; }

    // Writes the grid into a texture and draws one sprite — no per-cell geometry, one draw call.
    // setSmooth(true) on the texture also gets hardware bilinear filtering between cells for free.
    void render(sf::RenderWindow* window = nullptr)
    {
        for (uint32_t cy = 0; cy < CellsY; ++cy)
        {
            const float* __restrict row = &front[static_cast<size_t>(cy + 1) * PaddedW + 1];
            uint8_t* __restrict px = &pixel_buffer[static_cast<size_t>(cy) * CellsX * 4];

            for (uint32_t cx = 0; cx < CellsX; ++cx)
            {
                const float t = std::clamp(row[cx] / max_pheromone, 0.f, 1.f);
                const float g = 0.35f + t * 0.65f;

                px[0] = 0;
                px[1] = static_cast<uint8_t>(g * 255.f);
                px[2] = 0;
                px[3] = static_cast<uint8_t>((0.15f + t * 0.6f) * 255.f);
                px += 4;
            }
        }

        heat_texture.update(pixel_buffer.data());

		if (window != nullptr)
            window->draw(heat_sprite);
    }

    void track_stats()
    {
        float total = 0.f, max_v = 0.f;
        int active = 0;

        for (uint32_t cy = 0; cy < CellsY; ++cy)
        {
            const float* row = &front[static_cast<size_t>(cy + 1) * PaddedW + 1];
            for (uint32_t cx = 0; cx < CellsX; ++cx)
            {
                const float v = row[cx];
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
            max_pheromone > 0.f ? max_v * 100.f / max_pheromone : 0.f);
        ImGui::Text("Active    %d  (%.1f%%)", active, tc > 0.f ? active * 100.f / tc : 0.f);
    }

    sf::Texture get_texture() const { return heat_texture; }
	sf::Sprite  get_sprite() const { return heat_sprite; }
	sf::Vector2i get_cell_dimensions() const { return { static_cast<int>(CellsX), static_cast<int>(CellsY) }; }
	sf::Vector2f get_grid_size() const { return { cell_width, cell_height }; }

private:
    // Replicates edge cells into the 1-cell border so the diffusion kernel never bounds-checks.
    void replicate_border()
    {
        for (uint32_t y = 1; y <= CellsY; ++y)
        {
            front[y * PaddedW] = front[y * PaddedW + 1];
            front[y * PaddedW + PaddedW - 1] = front[y * PaddedW + CellsX];
        }
        for (uint32_t x = 0; x < PaddedW; ++x)
        {
            front[x] = front[PaddedW + x];
            front[(PaddedH - 1) * PaddedW + x] = front[CellsY * PaddedW + x];
        }
    }

    void resize_render_texture()
    {
        // SFML 3 renamed Texture::create to Texture::resize — swap back to create({w,h})
        // if you're still on SFML 2.
        if (!heat_texture.resize({ CellsX, CellsY }))
			throw std::runtime_error("Failed to resize pheromone heatmap texture.");

 
        heat_texture.setSmooth(true);
        heat_sprite.setTexture(heat_texture, true); // true = reset texture rect to the new size
        heat_sprite.setScale({ cell_width, cell_height });
        heat_sprite.setPosition({ 0.f, 0.f });

        pixel_buffer.assign(static_cast<size_t>(CellsX) * CellsY * 4, 0);
    }


    uint32_t CellsX = 0;
    uint32_t CellsY = 0;
    uint32_t PaddedW = 0;
    uint32_t PaddedH = 0;

    float cell_width = 0;
    float cell_height = 0;
    float world_width = 0;
    float world_height = 0;

    alignas(64) std::vector<float> front{};
    alignas(64) std::vector<float> back{};
    alignas(64) std::vector<float> h_pass{}; // scratch buffer for the horizontal diffusion pass

    sf::Texture heat_texture{};
    sf::Sprite  heat_sprite{ heat_texture };
    std::vector<uint8_t> pixel_buffer{};
};
