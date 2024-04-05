#pragma once

// A rectangle with rounded borders. Acts the same way a sf::RectangleShape does

#include <SFML/Graphics.hpp>
#include <cmath>

class RoundRect final : public sf::Drawable
{
public:
    RoundRect(const float width, const float height, const float roundness, const sf::Color color = sf::Color::White) :
        m_width_(width), m_height_(height), m_roundness_(roundness), m_color_(color)
	{
        update();
    }

    void set_size(const float width, const float height)
	{
        m_width_ = width;
        m_height_ = height;
        update();
    }

    void set_roundness(const float roundness)
	{
        m_roundness_ = roundness;
        update();
    }

    void set_color(const sf::Color color)
	{
        m_color_ = color;
        update();
    }

private:
    void update()
	{
        m_vertices_.clear();

        // Calculate the number of vertices in the corners based on the roundness
        const int cornerVertices = std::max(static_cast<int>(m_roundness_ * 0.25f), 1);

        // Create the four corners
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < cornerVertices; ++j) {
	            const float angle = (i * 90.f + j * 90.f / (cornerVertices - 1)) * 3.14159265359f / 180.f;
	            const float x = m_width_ * (i == 1 || i == 2 ? 1 - cos(angle) : cos(angle));
	            const float y = m_height_ * (i == 2 || i == 3 ? 1 - sin(angle) : sin(angle));
                m_vertices_.append(sf::Vertex(sf::Vector2f(x, y), m_color_));
            }
        }

        // Create the lines connecting the corners
        for (int i = 0; i < 4; ++i) {
            m_vertices_.append(sf::Vertex(sf::Vector2f(i % 2 == 0 ? 0 : m_width_, i < 2 ? 0 : m_height_), m_color_));
        }
    }

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states = sf::RenderStates()) const override
	{
        target.draw(m_vertices_, states);
    }

    float m_width_;
    float m_height_;
    float m_roundness_;
    sf::Color m_color_;
    sf::VertexArray m_vertices_;
};
#pragma once

// A rectangle with rounded borders. Acts the same way a sf::RectangleShape does

#include <SFML/Graphics.hpp>
#include <cmath>

class RoundRect final : public sf::Drawable
{
public:
    RoundRect(const float width, const float height, const float roundness, const sf::Color color = sf::Color::White) :
        m_width_(width), m_height_(height), m_roundness_(roundness), m_color_(color)
	{
        update();
    }

    void set_size(const float width, const float height)
	{
        m_width_ = width;
        m_height_ = height;
        update();
    }

    void set_roundness(const float roundness)
	{
        m_roundness_ = roundness;
        update();
    }

    void set_color(const sf::Color color)
	{
        m_color_ = color;
        update();
    }

private:
    void update()
	{
        m_vertices_.clear();

        // Calculate the number of vertices in the corners based on the roundness
        const int cornerVertices = std::max(static_cast<int>(m_roundness_ * 0.25f), 1);

        // Create the four corners
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < cornerVertices; ++j) {
	            const float angle = (i * 90.f + j * 90.f / (cornerVertices - 1)) * 3.14159265359f / 180.f;
	            const float x = m_width_ * (i == 1 || i == 2 ? 1 - cos(angle) : cos(angle));
	            const float y = m_height_ * (i == 2 || i == 3 ? 1 - sin(angle) : sin(angle));
                m_vertices_.append(sf::Vertex(sf::Vector2f(x, y), m_color_));
            }
        }

        // Create the lines connecting the corners
        for (int i = 0; i < 4; ++i) {
            m_vertices_.append(sf::Vertex(sf::Vector2f(i % 2 == 0 ? 0 : m_width_, i < 2 ? 0 : m_height_), m_color_));
        }
    }

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states = sf::RenderStates()) const override
	{
        target.draw(m_vertices_, states);
    }

    float m_width_;
    float m_height_;
    float m_roundness_;
    sf::Color m_color_;
    sf::VertexArray m_vertices_;
};
