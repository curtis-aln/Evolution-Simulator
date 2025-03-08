#pragma once

#include <SFML/Graphics.hpp>
#include "../Random.h"

inline sf::Texture generateCircleTexture(float radius)
{
    sf::RenderTexture renderTexture;
    const auto r = static_cast<unsigned>(radius * 2.f);
    renderTexture.create(r, r); // Create render texture

    sf::CircleShape circle(radius);
    circle.setFillColor(sf::Color::White);  // White so we can tint it with vertex colors
    circle.setOrigin(radius, radius);  // Center the origin
    circle.setPosition(radius, radius);

    renderTexture.clear(sf::Color::Transparent);
    renderTexture.draw(circle);
    renderTexture.display();

    return renderTexture.getTexture();
}


class CircleBuffer
{
    sf::RenderWindow* window_ = nullptr;
    sf::Texture texture;
    sf::VertexArray vertex_array{ sf::Quads };

public:
    CircleBuffer(sf::RenderWindow* window) : window_(window)
    {
    
    }

    void init_texture(std::vector<sf::Color>& colors, const float circle_radius, const int num_circles)
    {
        // Init the texture
        texture = generateCircleTexture(circle_radius);
        texture.setSmooth(true); // Enabled smoothing for better quality

        vertex_array.resize(static_cast<size_t>(num_circles * 4)); // 4 vertices per quad
        float tex_rad = texture.getSize().x;

        // These variables dont change during rendering so they are pre-set here
        for (size_t i = 0; i < num_circles; ++i)
        {
            size_t index = i * 4;

            // Set color (all white)
            sf::Color color = colors[i];
            color.a = 160;
            vertex_array[index + 0].color = color;
            vertex_array[index + 1].color = color;
            vertex_array[index + 2].color = color;
            vertex_array[index + 3].color = color;

            // Set texture coordinates
            const float new_rad = Random::rand_range(0.2f, 1.f) * tex_rad;
            vertex_array[index + 0].texCoords = sf::Vector2f(0, 0);
            vertex_array[index + 1].texCoords = sf::Vector2f(tex_rad, 0);
            vertex_array[index + 2].texCoords = sf::Vector2f(tex_rad, tex_rad);
            vertex_array[index + 3].texCoords = sf::Vector2f(0, tex_rad);
        }
    }


    void render(std::vector<sf::Vector2f>& positions_x)
    {
        size_t num_circles = positions_x.size();
        sf::Vector2f tex_size(texture.getSize());
        for (size_t i = 0; i < num_circles; ++i)
        {
            sf::Vector2f pos = positions_x[i];
            float r = tex_size.x / 2.0f; // Assuming a square texture

            size_t index = i * 4;

            // Define quad (textured sprite)
            vertex_array[index + 0].position = sf::Vector2f(pos.x - r, pos.y - r);
            vertex_array[index + 1].position = sf::Vector2f(pos.x + r, pos.y - r);
            vertex_array[index + 2].position = sf::Vector2f(pos.x + r, pos.y + r);
            vertex_array[index + 3].position = sf::Vector2f(pos.x - r, pos.y + r);
        }

        // Draw all circles in one batch
        window_->draw(vertex_array, &texture);
    }
};
