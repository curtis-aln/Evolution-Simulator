#include "world.h"
#include "../Utils/utility_SFML.h"
#include "../Utils/Graphics/CircleBatchRenderer.h"
#include "../protozoa/genetics/CellGenome.h"

World::World(sf::RenderWindow* window)
    : m_window_(window),
    world_border_renderer_(make_circle(world_circular_bounds_.radius, world_circular_bounds_.center)),
    thread_pool_(8)
{
    init_organisms();

    const size_t maximum_cells = max_protozoa * ProtozoaSettings::max_cells;

    render_data_.reserve(static_cast<int>(maximum_cells));
    distribution_.reserve(maximum_cells);
    cell_pointers_.resize(maximum_cells);
    inner_radii_.resize(maximum_cells);
    collision_resolutions.resize(maximum_cells);

    inner_circle_renderer_.init(90, maximum_cells);
    outer_circle_renderer_.init(90 + GraphicalSettings::cell_outline_thickness, maximum_cells);
}

void World::render(Font* font, sf::Vector2f mouse_pos)
{
    if (toggles.draw_cell_grid)
        cell_grid_renderer_.render(*m_window_, mouse_pos, 800.f);

    if (toggles.draw_food_grid)
        food_manager_.draw_food_grid(mouse_pos);

    food_manager_.render();
    render_protozoa(font);

    m_window_->draw(world_border_renderer_);
}

void World::render_protozoa(Font* font)
{
    outer_circle_renderer_.update_colors(render_data_.outer_colors, entity_count);
    outer_circle_renderer_.render(render_data_.positions, render_data_.radii, entity_count);

    if (!toggles.simple_mode)
    {
        for (int i = 0; i < entity_count; ++i)
            inner_radii_[i] = render_data_.radii[i] / GraphicalSettings::cell_outline_thickness;

        inner_circle_renderer_.update_colors(render_data_.inner_colors, entity_count);
        inner_circle_renderer_.render(render_data_.positions, inner_radii_, entity_count);
    }

    if (selected_protozoa_ != nullptr && toggles.debug_mode)
    {
        selected_protozoa_->render_debug(font, toggles.skeleton_mode,
            toggles.show_connections,
            toggles.show_bounding_boxes);
    }

    for (Protozoa* protozoa : all_protozoa_)
        protozoa->cell_positions_nearby.clear();
}

void World::init_organisms()
{
    for (int i = 0; i < max_protozoa; ++i)
        all_protozoa_.emplace({ i, &world_circular_bounds_, m_window_ });

    for (int i = initial_protozoa; i < max_protozoa; ++i)
    {
        all_protozoa_.at(i)->kill();
        all_protozoa_.at(i)->soft_reset();
        all_protozoa_.remove(i);
    }

    for (Protozoa* protozoa : all_protozoa_)
        generate_protozoa(*protozoa, world_circular_bounds_);
}

bool World::handle_mouse_click(const sf::Vector2f mouse_position)
{
    for (Protozoa* protozoa : all_protozoa_)
    {
        if (protozoa->check_mouse_press(mouse_position, true) != -1)
        {
            selected_protozoa_ = protozoa;
            return true;
        }
    }
    return false;
}

void World::keyboardEvents(const sf::Keyboard::Key& event_key_code)
{
    switch (event_key_code)
    {
    case sf::Keyboard::Key::G:
        toggles.draw_cell_grid = !toggles.draw_cell_grid;
        break;

    case sf::Keyboard::Key::C:
        if (toggles.debug_mode)
            toggles.show_connections = !toggles.show_connections;
        else
            toggles.toggle_collisions = !toggles.toggle_collisions;
        break;

    case sf::Keyboard::Key::F:
        toggles.draw_food_grid = !toggles.draw_food_grid;
        break;

    case sf::Keyboard::Key::S:
        toggles.simple_mode = !toggles.simple_mode;
        break;

    case sf::Keyboard::Key::D:
        toggles.debug_mode = !toggles.debug_mode;
        break;

    case sf::Keyboard::Key::T:
        toggles.track_statistics = !toggles.track_statistics;
        break;

    case sf::Keyboard::Key::K:
        if (toggles.debug_mode)
            toggles.skeleton_mode = !toggles.skeleton_mode;
        break;

    case sf::Keyboard::Key::B:
        if (toggles.debug_mode)
            toggles.show_bounding_boxes = !toggles.show_bounding_boxes;
        break;

    default:
        break;
    }
}

const std::vector<float>& World::get_generation_distribution()
{
    distribution_.clear();
    for (Protozoa* protozoa : all_protozoa_)
        for (const Cell& cell : protozoa->get_cells())
            distribution_.push_back(static_cast<float>(cell.generation));

    return distribution_;
}

void World::update_spatial_renderers()
{
    cell_grid_renderer_.rebuild();
    food_manager_.update_food_grid_renderer();
}

void World::unload_render_data(SimSnapshot& snapshot)
{
    // we check if the rendering process has written any new data for us to use, we unload it onto the world before updating it
    
}

void World::fill_snapshot(SimSnapshot& snapshot)
{
    snapshot.render = get_render_data();
    snapshot.stats = get_statistics();
    snapshot.toggles = toggles;
	snapshot.stats.protozoa_count = get_protozoa_count();
	snapshot.stats.food_count = get_food_count();
	snapshot.stats.average_generation = get_average_generation();

    if (selected_protozoa_ != nullptr)
    {
        snapshot.protozoa = *selected_protozoa_;
		snapshot.selected_a_protozoa = true;
    }
}