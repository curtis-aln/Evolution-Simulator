#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <filesystem>

#ifdef _WIN32
#include <windows.h>
#endif

// Helper: returns the directory containing the running executable
inline std::filesystem::path exe_dir()
{
#ifdef _WIN32
    wchar_t buf[MAX_PATH];
    GetModuleFileNameW(nullptr, buf, MAX_PATH);
    return std::filesystem::path(buf).parent_path();
#else
    return std::filesystem::canonical("/proc/self/exe").parent_path();
#endif
}

inline std::string resolve_media_path(const std::string& relative)
{
    // Try relative to exe first, fall back to cwd
    auto candidate = exe_dir() / relative;
    if (std::filesystem::exists(candidate))
        return candidate.string();
    return relative; // fallback — lets SFML produce its own error
}

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
    Font(sf::RenderWindow* window = nullptr, const unsigned font_size = 0,
        const std::string& font_location = "media/fonts/Roboto-Bold.ttf") : m_window_(window), m_text_{m_font_}
    {
        font_location_ = font_location;
        if (!font_location.empty())
        {
            set_font(resolve_media_path(font_location));
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
        const auto resolved = resolve_media_path(font_location);
        if (!m_font_.openFromFile(resolved))
        {
            std::cerr << "[ERROR]: Failed to load font from: " << resolved << '\n';
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
            m_text_.setOrigin(text_bounds.position + text_bounds.size / 2.0f);
        }
        else
        {
            m_text_.setOrigin(sf::Vector2f(0.0f, 0.0f)); // Reset origin when not centered
        }

        // Set text rotation around its center
        // SFML 3.x change: setRotation takes sf::Angle, use sf::degrees()
        m_text_.setRotation(sf::degrees(rotation));

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
        // SFML 3.x change: use getSize() instead of width/height
        return text_bounds.size;
    }
};
