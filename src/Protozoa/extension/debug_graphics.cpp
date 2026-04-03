#include "../Protozoa.h"
#include "../../settings.h"
#include "../../Utils/utility.h"

void Protozoa::render_protozoa_springs()
{
	for (Cell& cell : m_cells_)
	{
		render_cell_connections(cell, true);
	}
}

void Protozoa::render_debug(Font* font, const bool skeleton_mode, const bool show_connections, const bool show_bounding_boxes)
{
	// skeleton mode just hides the cell bodies and leaves only the outlines of the cells
	if (skeleton_mode)
	{
		draw_cell_outlines();
	}


	// This is the bounding box of the protozoa, used for external collision events
	if (show_bounding_boxes)
	{
		draw_protozoa_bounding_box(m_personal_bounds_, *m_window_);
	}

	draw_cell_physics(font);
	draw_spring_information(font);

	if (show_connections)
	{
		nearby_food_information();
	}
}

void Protozoa::draw_cell_outlines()
{
    sf::CircleShape circle_outline;
    circle_outline.setPointCount(30); // Reduce aliasing, set once
    for (Cell& cell : m_cells_)
    {
        const sf::Vector2f pos = cell.position_;
        const float rad = cell.radius + GraphicalSettings::cell_outline_thickness;

        circle_outline.setRadius(rad);
        circle_outline.setFillColor({ 0, 0, 0 });
        circle_outline.setPosition(pos - sf::Vector2f{rad, rad});
        m_window_->draw(circle_outline);

        circle_outline.setFillColor({ 255, 0, 255 });
        circle_outline.setRadius(rad / 3);
        circle_outline.setPosition(pos - sf::Vector2f{ rad / 3, rad / 3 });
        m_window_->draw(circle_outline);
    }
}

void Protozoa::nearby_food_information() const
{
	const sf::Vector2f center = get_center();
    static const sf::Color food_line_color{ 50, 153, 204, 100 };
    for (const sf::Vector2f& pos : food_positions_nearby)
    {
        m_window_->draw(make_line(center, pos, food_line_color));
    }

    for (const sf::Vector2f& pos : cell_positions_nearby)
    {
        m_window_->draw(make_line(center, pos, sf::Color::Yellow));
    }
}


void Protozoa::render_cell_connections(Cell& cell, const bool thick_lines) const
{
    for (const Spring& spring : m_springs_)
    {
        const sf::Vector2f& pos1 = m_cells_[spring.cell_A_id].position_;
        const sf::Vector2f& pos2 = m_cells_[spring.cell_B_id].position_;

        if (thick_lines)
        {
            draw_thick_line(*m_window_, pos1, pos2, GraphicalSettings::spring_thickness,
				GraphicalSettings::spring_outline_thickness, {255, 255, 255}, { 255, 255, 255 }); // todo
        }
        else
        {
            m_window_->draw(make_line(pos1, pos2, sf::Color::Magenta));
        }
    }
}


void Protozoa::draw_cell_physics(Font* font)
{
	// for each cell we draw its bounding box
    for (Cell& cell : m_cells_)
    {
        const sf::Vector2f& pos = cell.position_;
        const float rad = cell.radius;

        // rendering the bounding boxes
		const sf::FloatRect rect = { {pos.x - rad, pos.y - rad}, {rad * 2, rad * 2} };
        draw_protozoa_bounding_box(rect, *m_window_);
        render_cell_connections(const_cast<Cell&>(cell));

        // drawing the direction of the cell
        const float arrow_length = std::min(rad * 4, cell.velocity_.length() * rad);
        draw_direction(*m_window_, pos, cell.velocity_, arrow_length, 6, 10,
            { 200, 220, 200 }, { 190, 200, 190 });

        // drawing cell stats
        const auto top_left = rect.position;
        const auto spacing = font->get_text_size("0").y;
        const sf::Vector2f offset = { 0, spacing };
        font->draw(top_left, "id: " + std::to_string(cell.id), false);
        font->draw(top_left + offset, "phase: " + std::to_string(cell.sinwave_current_phase_), false);
        font->draw(top_left + offset * 2.f, "friction: " + std::to_string(cell.sinwave_current_friction_), false);
        font->draw(top_left + offset * 3.f, "gen: " + std::to_string(cell.generation), false);
    }
}




void Protozoa::draw_spring_information(Font* font) const
{
	// spring information
    for (const Spring& spring : m_springs_)
    {
        const sf::Vector2f& cell_A_pos = m_cells_[spring.cell_A_id].position_;
        const sf::Vector2f& cell_B_pos = m_cells_[spring.cell_B_id].position_;

		const sf::Vector2f mid_point = (cell_A_pos + cell_B_pos) * 0.5f;
		const sf::Vector2f upper_quartile = (mid_point + cell_B_pos) * 0.5f;
		const sf::Vector2f lower_quartile = (cell_A_pos + mid_point) * 0.5f;
		const sf::Vector2f interquartile_range = upper_quartile - lower_quartile;

        const sf::Vector2f& f1 = spring.direction_A_force;
        const sf::Vector2f& f2 = spring.direction_B_force;

        std::ostringstream vectorF1;
        vectorF1 << "force A: (" << denary_to_str(f1.x, 2) << ", " << denary_to_str(f1.y, 2) << ")";
        std::ostringstream vectorF2;
        vectorF2 << "force B: (" << denary_to_str(f2.x, 2) << ", " << denary_to_str(f2.y, 2) << ")";
		std::ostringstream length;
		length << "length: " << denary_to_str(spring.spring_length, 2);
		std::ostringstream breaking;
		breaking << "breaking: " << denary_to_str(spring.breaking_length, 2);
		std::ostringstream broken;
		broken << "broken: " << spring.broken;

        font->draw(lower_quartile, vectorF1.str(), true);
        font->draw(upper_quartile, vectorF2.str(), true);
		font->draw(lower_quartile + interquartile_range/3.f, length.str(), true);
		font->draw(upper_quartile - interquartile_range/3.f, breaking.str(), true);
		font->draw(upper_quartile - interquartile_range/2.f, broken.str(), true);
    }
}

// TODO: Implement spring connection creation
void Protozoa::make_connection(const int cell1_id, const int cell2_id)
{
	// todo
	//const SpringGene& genetics = create_cell_connection(cell1_id, cell2_id);
	//m_springs_.emplace_back(genetics.connecting_cell_ids, genetics.colors, genetics.rest_length,
	//	genetics.spring_constant, genetics.damping);
}


bool Protozoa::is_hovered_on(const sf::Vector2f mousePosition, const bool tolerance_check) const
{
    if (tolerance_check)
    {
        // When the mouse hovers over a protozoa, add extra to its boundary to make it easier to click
        constexpr float bounds_tolerance = 30.f;
        sf::FloatRect resized_rect = resize_rect(m_personal_bounds_, { -bounds_tolerance , -bounds_tolerance });
        return resized_rect.contains(mousePosition);
    }
    return m_personal_bounds_.contains(mousePosition);
}

bool Protozoa::check_press(const sf::Vector2f mouse_position)
{
	return m_personal_bounds_.contains(mouse_position);
}


Cell* Protozoa::get_selected_cell(const sf::Vector2f mouse_pos)
{
	if (!is_hovered_on(mouse_pos))
		return nullptr;
	
	for (Cell& cell : m_cells_)
	{
		const float dist_sq = (cell.position_ - mouse_pos).lengthSquared();
		const float rad = cell.radius;
		if (dist_sq < rad * rad)
		{
			return &cell;
		}
	}
	
	return nullptr;
}

std::vector<Cell>& Protozoa::get_cells()
{
	return m_cells_;
}


void Protozoa::set_render_window(sf::RenderWindow* window)
{
	m_window_ = window;
}

void Protozoa::set_bounds(Circle* bounds)
{
	m_world_bounds_ = bounds;
}
