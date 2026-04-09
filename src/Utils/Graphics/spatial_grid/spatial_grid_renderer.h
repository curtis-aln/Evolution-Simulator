#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include "simple_spatial_grid.h"

// ─── Settings ───────────────────────────────────────────────────────────────
namespace GridRendererSettings
{
    inline constexpr int   CHAR_SIZE = 12;
    inline constexpr float FONT_SCALE = 1.f;
    inline constexpr sf::Color GRID_LINE_COLOR = { 75, 75, 75 };
    inline constexpr sf::Color TEXT_COLOR_ACTIVE = sf::Color::White;
    inline constexpr sf::Color TEXT_COLOR_DIM = { 80, 80, 80 };
    inline const     std::string FONT_PATH = "media/fonts/Calibri.ttf";
}
// ────────────────────────────────────────────────────────────────────────────

class SpatialRenderer
{
    const SimpleSpatialGrid* grid = nullptr;

    sf::VertexBuffer vertexBuffer{};
    sf::Font         font;
    sf::Text         text{ font };

    // Cached from grid so we don't recompute every frame
    sf::Vector2f cell_dim{};
    size_t       cells_x = 0;
    size_t       cells_y = 0;

public:
    explicit SpatialRenderer(const SimpleSpatialGrid* grid_)
        : grid(grid_)
    {
        syncFromGrid();
        initFont();
        initVertexBuffer();
    }

    // Call this if the grid is resized / world changes at runtime
    void rebuild()
    {
        syncFromGrid();
        initVertexBuffer();
    }

    // query_pos  – world-space position of the mouse / probe point
    // query_radius – world-space radius; cells whose centre falls within it are
    //               highlighted and annotated; pass 0 to annotate all cells
    void render_grid(sf::RenderWindow& window,
        sf::Vector2f       query_pos,
        float              query_radius = 0.f)
    {
        window.draw(vertexBuffer);

        for (size_t x = 0; x < cells_x; ++x)
        {
            for (size_t y = 0; y < cells_y; ++y)
            {
                const cell_idx idx_1d = static_cast<cell_idx>(y * cells_x + x);

                // Cell centre in world space
                const sf::Vector2f top_left = { x * cell_dim.x, y * cell_dim.y };
                const sf::Vector2f centre = top_left + cell_dim * 0.5f;

                // Radius / proximity check ─────────────────────────────────
                const float dx = centre.x - query_pos.x;
                const float dy = centre.y - query_pos.y;
                const float dist_sq = dx * dx + dy * dy;
                const bool  in_range = (query_radius <= 0.f) ||
                    (dist_sq <= query_radius * query_radius);

                if (!in_range)
                    continue;

                // Object count for this cell ───────────────────────────────
                const uint8_t obj_count = grid->cell_capacities[idx_1d];

                // Build debug string ───────────────────────────────────────
                const std::string info =
                    "1D: " + std::to_string(idx_1d) + "\n" +
                    "2D: (" + std::to_string(x) + ", " + std::to_string(y) + ")\n" +
                    "TL: (" + fmtF(top_left.x) + ", " + fmtF(top_left.y) + ")\n" +
                    "objs: " + std::to_string(obj_count);

                text.setFillColor(obj_count > 0
                    ? GridRendererSettings::TEXT_COLOR_ACTIVE
                    : GridRendererSettings::TEXT_COLOR_DIM);

                text.setString(info);
                text.setPosition(top_left + sf::Vector2f{ 4.f, 4.f });
                window.draw(text);
            }
        }
    }

private:
    // ── Helpers ─────────────────────────────────────────────────────────────

    void syncFromGrid()
    {
        cells_x = grid->CellsX;
        cells_y = grid->CellsY;
        cell_dim = { grid->cell_width, grid->cell_height };
    }

    static std::string fmtF(float v)
    {
        // One-decimal-place float string without <format> / printf
        const int whole = static_cast<int>(v);
        const int frac = static_cast<int>(std::abs(v - static_cast<float>(whole)) * 10.f);
        return std::to_string(whole) + "." + std::to_string(frac);
    }

    void initVertexBuffer()
    {
        // (CellsX + CellsY) lines × 2 vertices each
        const size_t vertex_count = (cells_x + cells_y) * 2;
        std::vector<sf::Vertex> vertices(vertex_count);

        vertexBuffer = sf::VertexBuffer(sf::PrimitiveType::Lines,
            sf::VertexBuffer::Usage::Static);
        vertexBuffer.create(vertex_count);

        const float world_w = grid->world_width;
        const float world_h = grid->world_height;

        size_t vi = 0;

        for (size_t x = 0; x < cells_x; ++x)
        {
            const float px = static_cast<float>(x) * cell_dim.x;
            vertices[vi].position = { px, 0.f };
            vertices[vi + 1].position = { px, world_h };
            vi += 2;
        }

        for (size_t y = 0; y < cells_y; ++y)
        {
            const float py = static_cast<float>(y) * cell_dim.y;
            vertices[vi].position = { 0.f,   py };
            vertices[vi + 1].position = { world_w, py };
            vi += 2;
        }

        for (size_t i = 0; i < vi; ++i)
            vertices[i].color = GridRendererSettings::GRID_LINE_COLOR;

        vertexBuffer.update(vertices.data(), vertex_count, 0);
    }

    void initFont()
    {
        text.setCharacterSize(GridRendererSettings::CHAR_SIZE);

        if (!font.openFromFile(GridRendererSettings::FONT_PATH))
        {
            std::cerr << "[ERROR]: Failed to load font from: "
                << GridRendererSettings::FONT_PATH << '\n';
        }
    }
};