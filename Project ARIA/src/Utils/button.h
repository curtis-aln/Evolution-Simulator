#pragma once

#include <SFML/Graphics.hpp>
#include "time.h"
#include "font_renderer.hpp"

class Button
{
    sf::RenderWindow* m_window = nullptr;

    sf::RectangleShape m_shape_{};

    Font m_font_;
    std::string m_text;

    sf::Color m_button_color_idle_{};
    sf::Color m_button_color_active_{};

    bool m_is_mouse_over_;
    StopWatch stop_watch_{};

	float press_cooldown_ = 0.25f;
    float wait_time_ = 0.f;
    

public:
    explicit Button(sf::RenderWindow* window, const std::string& font_location,
        const sf::Rect<float> rect = {0, 0, 0, 0},
        const sf::Color button_color_idle   = sf::Color(20, 20, 20),
        const sf::Color button_color_active = sf::Color(50, 50, 50))
        : m_window(window), m_font_(window, 10, font_location), m_button_color_idle_(button_color_idle),
          m_button_color_active_(button_color_active),
          m_is_mouse_over_(false)
    {
        m_shape_.setPosition(rect.getPosition());
        m_shape_.setSize(rect.getSize());
        m_shape_.setFillColor(button_color_idle);
    }


    void init_font(const std::string& button_text, 
        const unsigned text_size = 10, const sf::Color text_fill_color = sf::Color::White)
    {
        m_text = button_text;
        m_font_.set_font_size(text_size);
        m_font_.set_fill_color(text_fill_color);
    }


    bool check_click(const sf::Vector2f& mouse_position)
    {
        const float delta_time = static_cast<float>(stop_watch_.get_delta());
        if (wait_time_ > 0)
        {
            wait_time_ -= delta_time;
            return false;
        }

        m_is_mouse_over_ = m_shape_.getGlobalBounds().contains(mouse_position);
        const bool pressed = m_is_mouse_over_ && sf::Mouse::isButtonPressed(sf::Mouse::Left);

        m_shape_.setFillColor(m_button_color_idle_);
        if (pressed)
        {
            wait_time_ = press_cooldown_;
            m_shape_.setFillColor(m_button_color_active_);
        }


        return pressed;
    }


    void draw()
    {
        m_window->draw(m_shape_);

    	const sf::Vector2f pos = m_shape_.getPosition();
    	const sf::Vector2f size = m_shape_.getSize();
        m_font_.draw({pos.x + size.x/2, pos.y + size.y/2}, m_text, true);
    }


    void set_text(const std::string& button_text)
    {
        m_text = button_text;
    }


    void set_position(const sf::Vector2f& position)
    {
        m_shape_.setPosition(position);
    }


    void set_size(const sf::Vector2f& size)
    {
        m_shape_.setSize(size);
    }

    void set_font_size(const float new_font_size)
    {
        m_font_.set_font_size(new_font_size);
    }

    void set_rect(const sf::Rect<float>& rect)
    {
        set_position(rect.getPosition());
        set_size(rect.getSize());
    }

    void set_rest_color(const sf::Color new_color)
    {
        m_button_color_idle_ = new_color;
    }

    void set_action_color(const sf::Color new_color)
    {
        m_button_color_active_ = new_color;
    }
};
