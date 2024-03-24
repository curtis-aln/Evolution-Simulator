#pragma once

#include <SFML/Graphics.hpp>
#include <functional>

class Button
{
    sf::Rect<float> m_rect_{};
    sf::RectangleShape m_shape_{};
    sf::Text m_text_{};
    sf::Font m_font_{};

    bool m_is_mouse_over_;

    // is only true for 1 frame
    bool m_clicked_ = false;

public:
    explicit Button(const sf::Rect<float> rect, const sf::Color button_fill_color = sf::Color(50, 50, 50))
        : m_rect_(rect), m_is_mouse_over_(false)
    {
        m_shape_.setPosition(rect.getPosition());
        m_shape_.setSize(rect.getSize());
        m_shape_.setFillColor(button_fill_color);
    }


    void init_font(const std::string& button_text, const std::string& file_location, 
        const unsigned text_size = 10, const sf::Color text_fill_color = sf::Color::White)
    {
        m_font_.loadFromFile(file_location);
        m_text_.setFont(m_font_);
        m_text_.setString(button_text);
        m_text_.setCharacterSize(text_size);
        m_text_.setFillColor(text_fill_color);
        m_text_.setPosition(m_rect_.left + 10.f, m_rect_.top + 10.f);
    }


    bool check_click(const sf::Vector2f& mouse_position)
    {
        if (m_shape_.getGlobalBounds().contains(mouse_position)) {
            m_is_mouse_over_ = true;
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                return true;
            }
        }
        else {
            m_is_mouse_over_ = false;
        }
        return false;
    }

    void draw(sf::RenderWindow& window) const
    {
        window.draw(m_shape_);
        window.draw(m_text_);
    }

    void set_text(const std::string& button_text)
    {
        m_text_.setString(button_text);
    }

    void set_position(const sf::Vector2f& position)
    {
        m_shape_.setPosition(position);
        m_text_.setPosition(position.x + 10, position.y + 10); // Adjust as needed
    }

    void set_size(const sf::Vector2f& size)
    {
        m_shape_.setSize(size);
    }

    bool is_clicked() const { return m_clicked_; }
};
