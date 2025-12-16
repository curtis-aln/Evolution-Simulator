#pragma once

#include <SFML/Graphics.hpp>
#include "../Protozoa.h"
#include "../../Utils/Graphics/sin_wave_grapher.h" // the class we made earlier

// Manages all sin-wave debug visualisation for a selected protozoa.
// This class intentionally owns no simulation data and never mutates state.
// Its only responsibility is to make evolved parameters interpretable.
class SinWaveManager
{
public:
    explicit SinWaveManager(const sf::Font& font);

    // Selection control
    void set_selected_protozoa(Protozoa* protozoa);
    void clear_selection();
    Protozoa* get_selected_protozoa() const;

    // Keeps time-based visuals in sync with simulation
    void update(float internal_clock);

    // Draws all graphs + connections for the selected protozoa
    void draw(sf::RenderTarget& target);

    // Debug controls
    void set_enabled(bool enabled);
    void set_radius(float radius);

private:
    Protozoa* m_selected_protozoa = nullptr; // non-owning
    bool m_enabled = true;

    float m_internal_clock = 0.f;
    float m_radius = 180.f;

    SinWaveVisualizer m_visualizer;

    // Rendering helpers
    void draw_cell_graphs(sf::RenderTarget& target);
    void draw_spring_graphs(sf::RenderTarget& target);

    void draw_connection(
        sf::RenderTarget& target,
        sf::Vector2f from,
        sf::Vector2f to,
        sf::Color color
    ) const;
};
