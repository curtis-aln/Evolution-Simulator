#pragma once

#include <SFML/Graphics.hpp>
#include "../Random.h"

inline sf::Texture generateCircleTexture(float radius)
{
    const auto r = static_cast<unsigned>(radius * 2.f);
    sf::RenderTexture renderTexture({ r, r });
    sf::CircleShape circle(radius);
    circle.setFillColor(sf::Color::White);
    circle.setOrigin({ radius, radius });
    circle.setPosition({ radius, radius });
    renderTexture.clear(sf::Color::Transparent);
    renderTexture.draw(circle);
    renderTexture.display();
    return renderTexture.getTexture();
}

class CircleBatchRenderer
{
    sf::RenderWindow* window_ = nullptr;
    sf::Texture texture;
    sf::VertexArray vertex_array{ sf::PrimitiveType::Triangles };
    int max_circles = 0;
    float tex_size = 0.f;

public:
    explicit CircleBatchRenderer(sf::RenderWindow* window) : window_(window) {}

    // Call ONCE at startup
    void init(float circle_radius, int max_circles_)
    {
        max_circles = max_circles_;
        texture = generateCircleTexture(circle_radius);
        texture.setSmooth(true);

        vertex_array.resize(static_cast<size_t>(max_circles) * 6);

        tex_size = static_cast<float>(texture.getSize().x);
        const float u0 = 0.f, v0 = 0.f;
        const float u1 = tex_size, v1 = tex_size;

        for (size_t i = 0; i < static_cast<size_t>(max_circles); ++i)
        {
            const size_t base = i * 6;
            vertex_array[base + 0].texCoords = { u0, v0 };
            vertex_array[base + 1].texCoords = { u1, v0 };
            vertex_array[base + 2].texCoords = { u1, v1 };
            vertex_array[base + 3].texCoords = { u0, v0 };
            vertex_array[base + 4].texCoords = { u1, v1 };
            vertex_array[base + 5].texCoords = { u0, v1 };
        }
    }

    // Call every frame before render() to update colors
    void update_colors(const std::vector<sf::Color>& colors, int count)
    {
        const size_t capped = std::min(static_cast<size_t>(count), static_cast<size_t>(max_circles));
        for (size_t i = 0; i < capped; ++i)
        {
            const size_t base = i * 6;
            for (size_t k = 0; k < 6; ++k)
                vertex_array[base + k].color = colors[i];
        }
    }

    // Call every frame to update positions and draw
    void render(const std::vector<sf::Vector2f>& positions,
        const std::vector<float>& radii,
        int count)
    {
        const size_t capped = std::min(static_cast<size_t>(count),
            static_cast<size_t>(max_circles));

        const float base_r = tex_size * 0.5f;

        auto write_pos = [&](size_t i, const sf::Vector2f& pos, float desired_r)
            {
                const float scale = desired_r / base_r;
                const float r = base_r * scale; // effectively desired_r

                const size_t base = i * 6;

                vertex_array[base + 0].position = { pos.x - r, pos.y - r };
                vertex_array[base + 1].position = { pos.x + r, pos.y - r };
                vertex_array[base + 2].position = { pos.x + r, pos.y + r };
                vertex_array[base + 3].position = { pos.x - r, pos.y - r };
                vertex_array[base + 4].position = { pos.x + r, pos.y + r };
                vertex_array[base + 5].position = { pos.x - r, pos.y + r };
            };

        for (size_t i = 0; i < capped; ++i)
            write_pos(i, positions[i], radii[i]);

        const sf::Vector2f offscreen(-1e9f, -1e9f);
        for (size_t i = capped; i < static_cast<size_t>(max_circles); ++i)
            write_pos(i, offscreen, 0.f);

        window_->draw(vertex_array, &texture);
    }
};