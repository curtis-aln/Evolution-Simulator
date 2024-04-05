#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>


struct TextPacket
{
    sf::Vector2f position{};
    std::string text{};
    float rotation{};
    bool centered{};
};


class Font
{
    sf::Font m_font_;
    sf::Text m_text_;
    sf::RenderWindow* m_window_;

public:
    // Constructor taking m_font_ size and file location
    Font(sf::RenderWindow* window = nullptr, const unsigned font_size = 0, const std::string& font_location = "") : m_window_(window)
    {
        if (!font_location.empty())
        {
            set_font(font_location);
        }
        set_fill_color();
        set_font_size(font_size);
    }

    void set_font_size(const unsigned font_size)
    {
        m_text_.setCharacterSize(font_size);
    }

    void set_fill_color(const sf::Color color = sf::Color::White)
    {
	    m_text_.setFillColor(color);
    }

    void set_font(const std::string& font_location)
    {
        if (!m_font_.loadFromFile(font_location))
        {
            std::cerr << "[ERROR]: Failed to load m_font_ from: " << font_location << '\n';
        }
        m_text_.setFont(m_font_);
    }

    void set_render_window(sf::RenderWindow* window)
    {
        m_window_ = window;
    }


    void draw(const sf::Vector2f& position, const std::string& string_text, const bool centered = false, const float rotation = 0.0f)
    {
        m_text_.setString(string_text);

        // Calculate text bounds
        const sf::FloatRect text_bounds = m_text_.getLocalBounds();
        const sf::Vector2f text_position = position;

        // Adjust position for centering if needed
        if (centered)
        {
            m_text_.setOrigin(text_bounds.left + text_bounds.width / 2.0f, text_bounds.top + text_bounds.height / 2.0f);
        }

        // Set text rotation around its center
        m_text_.setRotation(rotation);

        m_text_.setPosition(text_position);
        m_window_->draw(m_text_);
    }

    void draw(const TextPacket& text_packet)
    {
        draw(text_packet.position, text_packet.text, text_packet.centered, text_packet.rotation);
    }


    sf::Vector2f get_text_size(const std::string& string_text)
    {
        m_text_.setString(string_text);
        const sf::FloatRect text_bounds = m_text_.getLocalBounds();
        return {text_bounds.width, text_bounds.height};
    }
};
