#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>

class Font
{
    sf::Font m_font_;
    sf::Text m_text_;
    sf::RenderWindow* m_window_;

public:
    // Constructor taking m_font_ size and file location
    Font(sf::RenderWindow* window, const unsigned font_size, const std::string& font_location) : m_window_(window)
    {
        set_font(font_location);
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


	void draw(const sf::Vector2f& position, const std::string& string_text, const bool centered = false, 
        const sf::RenderStates& render_states = sf::RenderStates())
    {
        m_text_.setString(string_text);

        // Calculate m_text bounds
        const sf::FloatRect text_bounds = m_text_.getLocalBounds();
        sf::Vector2f text_position = position;

        // Adjust position for centering if needed
        if (centered)
        {
            text_position.x -= text_bounds.width / 2.0f;
            text_position.y -= text_bounds.height / 2.0f;
        }

        m_text_.setPosition(text_position);
        m_window_->draw(m_text_, render_states);
    }
};
