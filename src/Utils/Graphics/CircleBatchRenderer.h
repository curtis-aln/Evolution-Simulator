#pragma once

#include <SFML/Graphics.hpp>
#include "../Random.h"

// SFML 3: RenderTexture is constructed with a Vector2u size
inline sf::Texture generateCircleTexture(float radius)
{
    const auto r = static_cast<unsigned>(radius * 2.f);

    sf::RenderTexture renderTexture({ r, r });

    sf::CircleShape circle(radius);
    circle.setFillColor(sf::Color::White);          // White so we can tint with vertex colors
    circle.setOrigin({ radius, radius });           // Center origin
    circle.setPosition({ radius, radius });

    renderTexture.clear(sf::Color::Transparent);
    renderTexture.draw(circle);
    renderTexture.display();

    return renderTexture.getTexture();
}

// Triangles version: each "quad" becomes 2 triangles => 6 vertices per circle
class CircleBatchRenderer
{
    sf::RenderWindow* window_ = nullptr;
    sf::Texture texture;

    sf::VertexArray vertex_array{ sf::PrimitiveType::Triangles };

    int max_circles = 0;

public:
    explicit CircleBatchRenderer(sf::RenderWindow* window) : window_(window) {}

    void init_texture(std::vector<sf::Color>& colors, float circle_radius, int num_circles)
    {
        max_circles = num_circles;

        texture = generateCircleTexture(circle_radius);
        texture.setSmooth(true);

        // 6 vertices per circle (2 triangles)
        vertex_array.resize(static_cast<size_t>(num_circles) * 6);

        const float tex_size = static_cast<float>(texture.getSize().x); // square texture

        // Pre-set per-circle stuff that doesn't change at render time (color + texcoords)
        for (size_t i = 0; i < static_cast<size_t>(num_circles); ++i)
        {
            const size_t base = i * 6;
            const sf::Color color = colors[i];

            // Per-vertex color
            for (size_t k = 0; k < 6; ++k)
                vertex_array[base + k].color = color;

            const float u0 = 0.f, v0 = 0.f;
            const float u1 = tex_size, v1 = tex_size; // always sample the full texture

            // Triangle 1: TL, TR, BR
            vertex_array[base + 0].texCoords = { u0, v0 };
            vertex_array[base + 1].texCoords = { u1, v0 };
            vertex_array[base + 2].texCoords = { u1, v1 };
            // Triangle 2: TL, BR, BL
            vertex_array[base + 3].texCoords = { u0, v0 };
            vertex_array[base + 4].texCoords = { u1, v1 };
            vertex_array[base + 5].texCoords = { u0, v1 };

            // Quad corners in tex space:
            // TL (u0,v0), TR (u1,v0), BR (u1,v1), BL (u0,v1)
            //
            // Triangle 1: TL, TR, BR
            vertex_array[base + 0].texCoords = { u0, v0 };
            vertex_array[base + 1].texCoords = { u1, v0 };
            vertex_array[base + 2].texCoords = { u1, v1 };
            // Triangle 2: TL, BR, BL
            vertex_array[base + 3].texCoords = { u0, v0 };
            vertex_array[base + 4].texCoords = { u1, v1 };
            vertex_array[base + 5].texCoords = { u0, v1 };
        }
    }

    void render(std::vector<sf::Vector2f>& positions, const float positions_size)
    {
        const size_t num_circles = positions_size;
        const sf::Vector2f tex_size_f(static_cast<float>(texture.getSize().x),
                                      static_cast<float>(texture.getSize().y));
        const float r = tex_size_f.x * 0.5f; // assuming square

        auto write_circle_positions = [&](size_t i, const sf::Vector2f& pos)
        {
            const size_t base = i * 6;

            const sf::Vector2f tl(pos.x - r, pos.y - r);
            const sf::Vector2f tr(pos.x + r, pos.y - r);
            const sf::Vector2f br(pos.x + r, pos.y + r);
            const sf::Vector2f bl(pos.x - r, pos.y + r);

            // Triangle 1: TL, TR, BR
            vertex_array[base + 0].position = tl;
            vertex_array[base + 1].position = tr;
            vertex_array[base + 2].position = br;

            // Triangle 2: TL, BR, BL
            vertex_array[base + 3].position = tl;
            vertex_array[base + 4].position = br;
            vertex_array[base + 5].position = bl;
        };

        // Write active circles
        const size_t capped = std::min(num_circles, static_cast<size_t>(max_circles));
        for (size_t i = 0; i < capped; ++i)
            write_circle_positions(i, positions[i]);

        // Push any unused circles off-screen (so you still draw max_circles in one call)
        // (You could also shrink the vertex array and avoid this, but this matches your current approach.)
        const sf::Vector2f offscreen(-1e9f, -1e9f);
        for (size_t i = capped; i < static_cast<size_t>(max_circles); ++i)
            write_circle_positions(i, offscreen);

        // Draw in one batch
        window_->draw(vertex_array, &texture);
    }
};
