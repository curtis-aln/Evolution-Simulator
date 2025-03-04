#pragma once

#include <SFML/Graphics.hpp>
#include "../time.h"
#include "../font_renderer.hpp"

class Button
{
    sf::RenderWindow* m_window_ = nullptr;

    sf::RectangleShape m_shape_{};

    Font m_font_;
    std::string m_text_;

    sf::Color m_color_idle_{};
    sf::Color m_color_active_{};

    bool m_mouse_hovering_ = false;
    StopWatch m_stop_watch_{};

	float m_press_cooldown_ = 0.25f;
    float m_wait_time_ = 0.f;

    bool m_is_font_initialised_ = false;
    bool m_is_graphics_initialised_ = false;
    

public:
    explicit Button(
        sf::RenderWindow* window = nullptr, const sf::Rect<float> rect = {})
	: m_window_(window), m_font_(window)
    {
        m_shape_.setPosition(rect.getPosition());
        m_shape_.setSize(rect.getSize());
    }


    void init_font(
        const std::string& button_text = "Button", 
        const std::string& font_location = "src/Utils/fonts/Roboto-Bold.ttf",
        const sf::Color text_fill_color = sf::Color::White,
        const unsigned text_size = 15)
    {
        m_text_ = button_text;
        m_font_.set_font(font_location);
        m_font_.set_font_size(text_size);
        m_font_.set_render_window(m_window_);
        m_font_.set_fill_color(text_fill_color);

        m_is_font_initialised_ = true;
    }


    void init_graphics(
        const sf::Color color_idle = sf::Color(20, 20, 20),
        const sf::Color color_active = sf::Color(50, 50, 50),
        const sf::Color outline_color = sf::Color(50, 50, 50),
        const float outline_thickness = 0)
    {
        m_color_idle_ = color_idle;
        m_color_active_ = color_active;
        m_shape_.setFillColor(color_idle);
        m_shape_.setOutlineColor(outline_color);
        m_shape_.setOutlineThickness(outline_thickness);

        m_is_graphics_initialised_ = true;
    }


    bool check_click(const sf::Vector2f& mouse_position)
    {
        const float delta_time = static_cast<float>(m_stop_watch_.get_delta());
        if (m_wait_time_ > 0)
        {
            m_wait_time_ -= delta_time;
            return false;
        }

        m_mouse_hovering_ = m_shape_.getGlobalBounds().contains(mouse_position);
        const bool pressed = m_mouse_hovering_ && sf::Mouse::isButtonPressed(sf::Mouse::Left);

        m_shape_.setFillColor(m_color_idle_);
        if (pressed)
        {
            m_wait_time_ = m_press_cooldown_;
            m_shape_.setFillColor(m_color_active_);
        }

        return pressed;
    }


    void draw()
    {
        check_init_error();
        m_window_->draw(m_shape_);

    	const sf::Vector2f pos = m_shape_.getPosition();
    	const sf::Vector2f size = m_shape_.getSize();
        m_font_.draw({pos.x + size.x/2, pos.y + size.y/2}, m_text_, true);
    }


    void set_text(const std::string& button_text)
    {
        m_text_ = button_text;
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
        m_color_idle_ = new_color;
    }
	void set_action_color(const sf::Color new_color)
    {
        m_color_active_ = new_color;
    }

    sf::Rect<float> get_rect() const
    {
        const sf::Vector2f pos = m_shape_.getPosition();
        const sf::Vector2f size = m_shape_.getSize();
        return { pos.x, pos.y, size.x, size.y };
    }


private:
    void check_init_error() const
    {
        if (!m_is_font_initialised_)
        {
            throw std::runtime_error("Font is not initialised");
        }

        if (!m_is_graphics_initialised_)
        {
            throw std::runtime_error("Graphics are not initialised");
        }
    }
};
