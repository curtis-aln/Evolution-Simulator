#include "SinWaveManager.h"
#include <cmath>

SinWaveManager::SinWaveManager(const sf::Font& font)
    : m_visualizer(font)
{
    // These defaults are chosen to make instability obvious,
    // not to look aesthetically pleasing.
    SinWaveSettings settings;
    settings.x_min = 0.f;
    settings.x_max = 10.f;
    settings.y_min = -1.5f;
    settings.y_max = 1.5f;
    settings.sample_count = 300;
    settings.enable_phase_line = true;

    m_visualizer.set_settings(settings);
}


void SinWaveManager::set_selected_protozoa(Protozoa* protozoa)
{
    m_selected_protozoa = protozoa;
}

void SinWaveManager::clear_selection()
{
    m_selected_protozoa = nullptr;
}

Protozoa* SinWaveManager::get_selected_protozoa() const
{
    return m_selected_protozoa;
}


void SinWaveManager::update(float internal_clock)
{
    // Time is injected explicitly to avoid hidden coupling
    // with any particular simulation clock.
    m_internal_clock = internal_clock;
}


void SinWaveManager::draw(sf::RenderTarget& target)
{
    if (!m_enabled || !m_selected_protozoa)
        return;

    draw_cell_graphs(target);
    draw_spring_graphs(target);
}


void SinWaveManager::draw_cell_graphs(sf::RenderTarget& target)
{
    auto& cells = m_selected_protozoa->get_cells();
    const sf::Vector2f center = m_selected_protozoa->get_center();

    const int count = static_cast<int>(cells.size());
    if (count == 0) return;

    for (int i = 0; i < count; ++i)
    {
        Cell& cell = cells[i];
        CellGene* gene = m_selected_protozoa->get_cell_gene(cell.id);
        if (!gene) continue;

        // Even angular spacing prevents visual bias when cell count changes.
        float angle = i * (2.f * 3.1415926f / count);

        sf::Vector2f graph_pos = center + sf::Vector2f(
            std::cos(angle),
            std::sin(angle)
        ) * m_radius;

        sf::View original = target.getView();

        // A temporary view isolates graph rendering from world scale.
        sf::View graph_view = original;
        graph_view.setCenter(graph_pos);
        graph_view.setSize({ 300.f, 200.f });
        target.setView(graph_view);

        m_visualizer.draw(
            target,
            m_internal_clock,
            gene->amplitude,
            gene->frequency,
            gene->offset,
            gene->vertical_shift
        );

        target.setView(original);

        draw_connection(
            target,
            graph_pos,
            cell.position_,
            sf::Color::Green
        );
    }
}

void SinWaveManager::draw_spring_graphs(sf::RenderTarget& target)
{
    auto& springs = m_selected_protozoa->get_springs();
    auto& cells = m_selected_protozoa->get_cells();
    const sf::Vector2f center = m_selected_protozoa->get_center();

    const int count = static_cast<int>(springs.size());
    if (count == 0) return;

    for (int i = 0; i < count; ++i)
    {
        Spring& spring = springs[i];
        SpringGene* gene = m_selected_protozoa->get_spring_gene(spring.id);
        if (!gene) continue;

        // Offset angle slightly so spring graphs don't overlap cell graphs.
        float angle = i * (2.f * 3.1415926f / count) + 0.5f;

        sf::Vector2f graph_pos = center + sf::Vector2f(
            std::cos(angle),
            std::sin(angle)
        ) * (m_radius + 60.f);

        sf::View original = target.getView();
        sf::View graph_view = original;
        graph_view.setCenter(graph_pos);
        graph_view.setSize({ 300.f, 200.f });
        target.setView(graph_view);

        m_visualizer.draw(
            target,
            m_internal_clock,
            gene->amplitude,
            gene->frequency,
            gene->offset,
            gene->vertical_shift
        );

        target.setView(original);

        // Midpoint communicates which connection this spring controls.
        const sf::Vector2f midpoint =
            (cells[spring.cell_A_id].position_ +
                cells[spring.cell_B_id].position_) * 0.5f;

        draw_connection(
            target,
            graph_pos,
            midpoint,
            sf::Color::Red
        );
    }
}


void SinWaveManager::draw_connection(
    sf::RenderTarget& target,
    sf::Vector2f from,
    sf::Vector2f to,
    sf::Color color
) const
{
    // Simple lines are intentional: thickness would imply strength,
    // which is misleading for purely informational links.
    sf::Vertex line[2] = {
        { from, color },
        { to,   color }
    };
    target.draw(line, 2, sf::Lines);
}


void SinWaveManager::set_enabled(bool enabled)
{
    m_enabled = enabled;
}

void SinWaveManager::set_radius(float radius)
{
    m_radius = radius;
}
