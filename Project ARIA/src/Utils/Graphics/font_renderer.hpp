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
    std::string font_location_;

public:
    // Constructor taking m_font_ size and file location
    Font(sf::RenderWindow* window = nullptr, const unsigned font_size = 0,  // todo make path relative
        const std::string& font_location = "media/fonts/Roboto-Bold.ttf") : m_window_(window)
    {
        font_location_ = font_location;
        if (!font_location.empty())
        {
            set_font(font_location);
            set_font_size(font_size);
        }
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
		font_location_ = font_location;
        if (!m_font_.loadFromFile(font_location))
        {
            std::cerr << "[ERROR]: Failed to load m_font_ from: " << font_location << '\n';
        }
        m_text_.setFont(m_font_);
    }

    std::string& get_font_location()
    {
        return font_location_;
	}

    void set_render_window(sf::RenderWindow* window)
    {
        m_window_ = window;
    }


    void draw(const sf::Vector2f& position, const std::string& string_text, const bool centered = false, const float rotation = 0.0f, sf::RenderWindow* render_window = nullptr)
    {
        if (string_text.empty())
            return;

        m_text_.setString(string_text);

        // Adjust position for centering if needed
        if (centered)
        {
            const sf::FloatRect text_bounds = m_text_.getLocalBounds();
            m_text_.setOrigin(text_bounds.left + text_bounds.width / 2.0f, text_bounds.top + text_bounds.height / 2.0f);
        }
        else
        {
            m_text_.setOrigin(0.0f, 0.0f); // Reset origin when not centered
        }

        // Set text rotation around its center
        m_text_.setRotation(rotation);

        m_text_.setPosition(position);

        // Draw using the specified window or the default one
        if (render_window != nullptr)
        {
            render_window->draw(m_text_);
        }
        else
        {
            m_window_->draw(m_text_);
        }
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
