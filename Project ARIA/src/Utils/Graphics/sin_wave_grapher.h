#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>
#include <string>

struct SinWaveSettings
{
    float x_min = 0.f;
    float x_max = 10.f;
    float y_min = -2.f;
    float y_max = 2.f;

    unsigned int sample_count = 500;

    bool enable_negative_x = false;
    bool enable_phase_line = true;
    bool enabled = true;

    sf::Color wave_colour = sf::Color::Cyan;
    sf::Color axis_colour = sf::Color(180, 180, 180);

    std::string x_label = "Time";
    std::string y_label = "Amplitude";
};


class SinWaveVisualizer
{
public:
    explicit SinWaveVisualizer(const sf::Font& font)
        : m_font(font)
    {
    }

    // Settings are external so evolution/debug tools can tweak them live
    void set_settings(const SinWaveSettings& settings)
    {
        m_settings = settings;
    }

    const SinWaveSettings& get_settings() const
    {
        return m_settings;
    }

    // Stateless draw call keeps this class reusable and predictable
    void draw(
        sf::RenderTarget& target,
        float internal_clock,
        float amplitude,
        float frequency,
        float phase_offset,
        float vertical_shift
    ) const
    {
        if (!m_settings.enabled)
            return;

        const sf::Vector2f view_size = target.getView().getSize();

        const sf::FloatRect screen_rect(
            60.f,
            30.f,
            view_size.x - 90.f,
            view_size.y - 90.f
        );

        const sf::FloatRect world_rect(
            m_settings.x_min,
            m_settings.y_min,
            m_settings.x_max - m_settings.x_min,
            m_settings.y_max - m_settings.y_min
        );

        draw_axes(target, world_rect, screen_rect);
        draw_wave(target, world_rect, screen_rect,
            internal_clock, amplitude, frequency,
            phase_offset, vertical_shift);
        draw_labels(target, screen_rect);
    }

private:
    const sf::Font& m_font;
    SinWaveSettings m_settings;

    static sf::Vector2f map_world_to_screen(
        float x,
        float y,
        const sf::FloatRect& world,
        const sf::FloatRect& screen)
    {
        // Normalisation prevents accidental coupling between
        // simulation scale and window resolution.
        float nx = (x - world.left) / world.width;
        float ny = (y - world.top) / world.height;

        return {
            screen.left + nx * screen.width,
            screen.top + (1.f - ny) * screen.height
        };
    }

    void draw_axes(
        sf::RenderTarget& target,
        const sf::FloatRect& world,
        const sf::FloatRect& screen
    ) const
    {
        sf::VertexArray axes(sf::Lines, 4);

        axes[0].position = map_world_to_screen(m_settings.x_min, 0.f, world, screen);
        axes[1].position = map_world_to_screen(m_settings.x_max, 0.f, world, screen);
        axes[2].position = map_world_to_screen(0.f, m_settings.y_min, world, screen);
        axes[3].position = map_world_to_screen(0.f, m_settings.y_max, world, screen);

        for (int i = 0; i < 4; ++i)
            axes[i].color = m_settings.axis_colour;

        target.draw(axes);
    }

    void draw_wave(
        sf::RenderTarget& target,
        const sf::FloatRect& world,
        const sf::FloatRect& screen,
        float internal_clock,
        float amplitude,
        float frequency,
        float phase_offset,
        float vertical_shift
    ) const
    {
        sf::VertexArray wave(sf::LineStrip, m_settings.sample_count);

        for (unsigned int i = 0; i < m_settings.sample_count; ++i)
        {
            float t = static_cast<float>(i) / (m_settings.sample_count - 1);

            float x = m_settings.enable_negative_x
                ? m_settings.x_min + t * (m_settings.x_max - m_settings.x_min)
                : t * m_settings.x_max;

            // The gene equation is kept intact so
            // visual behaviour always matches simulation behaviour.
            float y = amplitude * std::sinf(frequency * x + phase_offset)
                + vertical_shift;

            wave[i].position = map_world_to_screen(x, y, world, screen);
            wave[i].color = m_settings.wave_colour;
        }

        target.draw(wave);

        if (m_settings.enable_phase_line)
            draw_phase_line(target, world, screen, internal_clock, frequency, phase_offset);
    }

    void draw_phase_line(
        sf::RenderTarget& target,
        const sf::FloatRect& world,
        const sf::FloatRect& screen,
        float internal_clock,
        float frequency,
        float phase_offset
    ) const
    {
        sf::VertexArray line(sf::Lines, 2);

        // Phase wrapped into X-range so temporal drift
        // remains visible without leaving the graph.
        float phase_x = std::fmod(frequency * internal_clock + phase_offset,
            m_settings.x_max);

        line[0].position = map_world_to_screen(phase_x, m_settings.y_min, world, screen);
        line[1].position = map_world_to_screen(phase_x, m_settings.y_max, world, screen);

        line[0].color = sf::Color::Red;
        line[1].color = sf::Color::Red;

        target.draw(line);
    }

    void draw_labels(
        sf::RenderTarget& target,
        const sf::FloatRect& screen
    ) const
    {
        sf::Text x_text(m_settings.x_label, m_font, 16);
        x_text.setFillColor(sf::Color::White);
        x_text.setOrigin(x_text.getLocalBounds().width / 2.f, 0.f);
        x_text.setPosition(
            screen.left + screen.width / 2.f,
            screen.top + screen.height + 30.f
        );
        target.draw(x_text);

        sf::Text y_text(m_settings.y_label, m_font, 16);
        y_text.setFillColor(sf::Color::White);

        // Rotating the label forces you to think in world space,
        // not screen space — which prevents sign bugs later.
        y_text.setRotation(-90.f);
        y_text.setOrigin(y_text.getLocalBounds().width / 2.f, 0.f);
        y_text.setPosition(
            screen.left - 40.f,
            screen.top + screen.height / 2.f
        );
        target.draw(y_text);
    }
};
