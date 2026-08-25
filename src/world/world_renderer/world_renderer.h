#pragma once

#include <SFML/Graphics/RenderWindow.hpp>

#include "../world_settings.h"
#include "../connection_renderer.h"
#include "../world_border.h"

#include "../../Utils/Graphics/SFML_Grid.h"
#include "../../Utils/Graphics/CircleBatchRenderer.h"
#include "../../Utils/Graphics/font_renderer.hpp"

#include "../../Utils/utility_SFML.h"

#include "../collision_resolver/collision_resolver.h"

#include "../../managers/cell_manager/cell_manager_settings.h"
#include "../../managers/food_manager/food_manager.h"

#include "../../simulation/context/sim_snapshot.h"
#include "../..//simulation/settings/settings.h"

inline static constexpr int cells_div_value = 4; // cells_x / cells_div_value is the number of cells in the visual grid
inline static constexpr float world_border_thickness = 68.f; // thickness of the circular world border
inline static const sf::Color border_color = { 170, 200, 255, 100 }; // color of the circular world border
inline static constexpr int border_point_count = 256;

/* This class renders pretty much the entire simulation, including the world, cells, and other entities. */
class WorldRenderer : public WorldSettings
{
	sf::RenderWindow* m_window_ = nullptr;

	// this is used as a frame of reference to see how fast cells are moving
	size_t cells_along_axis_ = static_cast<size_t>(CollisionResolver::cells_x / cells_div_value);
	SFML_Grid visual_grid_;

	// This is the circular world border that is drawn on the screen
	sf::CircleShape world_border_renderer_{};

	// Rendering the Cells, Food, and Cell matter
	CircleBatchRenderer outer_circle_renderer_{};
	CircleBatchRenderer inner_circle_renderer_{};
	std::vector<float>  outer_radii_{};
	std::vector<sf::Vector2f>  outer_positions_{};
	std::vector<sf::Color>  colors_{};

	// rendering the springs
	ConnectionRenderer connection_renderer_{};

	// rendering the spatial grids
	SpatialGridRenderer collision_grid_renderer_;
	SpatialGridRenderer food_grid_renderer_;
	SpatialGridRenderer newborn_grid_renderer_;


public:
	// Constructor
	WorldRenderer(
		sf::RenderWindow* window, 
		SimpleSpatialGrid* collision_grid,
		SimpleSpatialGrid* food_grid,
		SimpleSpatialGrid* newborn_grid,
		sf::FloatRect& bounds_rect, 
		WorldBorder& circular_bounds)
		: 
		m_window_(window), 
		visual_grid_(*m_window_, bounds_rect, cells_along_axis_, 3, grid_color, grid_line_thickness),
		collision_grid_renderer_(collision_grid),
		food_grid_renderer_(food_grid),
		newborn_grid_renderer_(newborn_grid)
	{
		init_circle_renderers();
		init_world_border_renderer(circular_bounds);
	}

	void render(const SimSnapshot& snapshot, const sf::Vector2f mouse_pos)
	{
		render_visual_grid(snapshot);              // renders the faint grid in the background of the simulation
		render_spatial_grids(snapshot, mouse_pos); // renders the spatial grids for food, collision, and newborn cells if enabled
		render_protozoa(snapshot);                 // renders the protozoa (cells) and their springs
		render_influence_radii(snapshot);          // renders a circle around the mouse to show its influence radius when adding or removing entities

		m_window_->draw(world_border_renderer_);  
	}

private:
	void render_influence_radii(const SimSnapshot& snapshot)
	{
		// renders a circle around the mouse to show its influence radius when adding or removing entities
		if (!snapshot.world_toggles.show_influence_radius)
			return;

		sf::Vector2f mouse_pos = {snapshot.sim_stats.mouse_pos_x, snapshot.sim_stats.mouse_pos_y};
		float influence_radius = snapshot.world_stats.mouse_radius;

		sf::CircleShape influence_circle;
		influence_circle.setPointCount(60);
		influence_circle.setRadius(influence_radius);
		influence_circle.setFillColor(sf::Color(0, 0, 0, 0));
		influence_circle.setOutlineColor(sf::Color(255, 255, 255, 100));
		influence_circle.setOutlineThickness(2.f + influence_radius / 150.f);
		influence_circle.setPosition(mouse_pos - sf::Vector2f(influence_radius, influence_radius));

		m_window_->draw(influence_circle);
	}

	void init_circle_renderers()
	{
		/* Initialize the circle renderers for the outer and inner circles. */
		constexpr float texture_radius = 120.f;
		outer_circle_renderer_.init(m_window_, texture_radius, CellManagerSettings::max_protozoa);
		inner_circle_renderer_.init(m_window_, texture_radius, CellManagerSettings::max_protozoa);
	}

	void init_world_border_renderer(const WorldBorder& circular_bounds)
	{
		world_border_renderer_.setRadius(circular_bounds.bounds_radius);
		world_border_renderer_.setFillColor(sf::Color(0, 0, 0, 0));
		world_border_renderer_.setPointCount(border_point_count);
		world_border_renderer_.setOutlineColor(border_color);
		world_border_renderer_.setOutlineThickness(world_border_thickness);
	}

	void render_visual_grid(const SimSnapshot& snapshot)
	{
		// The visual grid is the faint grid that is drawn in the background of the simulation. 
		// It is used to help visualize the scale of the simulation and to help with debugging.
		float zoom = snapshot.sim_stats.camera_zoom;
		float a = 1.f;
		if (zoom < start_fading_zoom)
		{
			a = (zoom - start_fading_zoom) / fade_zoom_dist;
			a = std::clamp(a, 0.f, 1.f);
		}
		visual_grid_.draw(a);
	}

	void render_spatial_grids(const SimSnapshot& snapshot, const sf::Vector2f mouse_pos)
	{
		constexpr float query_radius = 500.f; 
		if (snapshot.world_toggles.draw_food_grid)
			food_grid_renderer_.render(*m_window_, mouse_pos, query_radius);

		if (snapshot.world_toggles.draw_collision_grid)
			collision_grid_renderer_.render(*m_window_, mouse_pos, query_radius);

		if (snapshot.cell_toggles.show_newborn_grid)
			newborn_grid_renderer_.render(*m_window_, mouse_pos, query_radius);
	}


	void render_protozoa(const SimSnapshot& snapshot)
	{
		int size = snapshot.render.positions.size();

		// The springs are rendered first, so they appear behind the cells in the rendering order.
		render_springs(snapshot);
		
		const float zoom = snapshot.sim_stats.camera_zoom;
		bool simplify_colors = zoom < 0.05f;

		if (!simplify_colors)
		{
			inner_circle_renderer_.set_size(size);
			inner_circle_renderer_.set_colors(simplify_colors ? colors_ : snapshot.render.inner_colors);
			inner_circle_renderer_.set_positions(snapshot.render.positions);
			inner_circle_renderer_.set_radii(snapshot.render.radii);

			inner_circle_renderer_.update();
			inner_circle_renderer_.render();
		}

		outer_radii_.resize(size);
		outer_positions_.resize(size);

		for (int i = 0; i < size; ++i)
		{
			auto pos = snapshot.render.positions[i];
			auto vel = snapshot.render.velocities[i];
			const float base_radius = snapshot.render.radii[i];
			const float rad = base_radius * GraphicalSettings::cell_outline_thickness;
			const float margin = rad - base_radius; // available slack in the outline ring

			// Ease-out: strong response near the centre (t≈0), tapering to zero
			// additional movement as the offset nears the outer edge of the ring (t≈1).
			auto ease_out = [margin](float v) -> float
				{
					if (margin <= 0.f)
						return 0.f;

					const float sign = (v < 0.f) ? -1.f : 1.f;
					const float t = std::clamp(std::abs(v) / margin, 0.f, 1.f);
					const float eased = 1.f - (1.f - t) * (1.f - t); // 1 - (1-t)^2
					return sign * eased * margin;
				};

			const float scaled_x = ease_out(vel.x);
			const float scaled_y = ease_out(vel.y);

			outer_positions_[i] = pos - sf::Vector2f{ scaled_x, scaled_y };
			outer_radii_[i] = rad;
		}

		outer_circle_renderer_.set_size(size);
		outer_circle_renderer_.set_colors(snapshot.render.outer_colors);
		outer_circle_renderer_.set_positions(outer_positions_);
		outer_circle_renderer_.set_radii(outer_radii_);

		outer_circle_renderer_.update();
		outer_circle_renderer_.render();

		// If a protozoa is selected and debug mode is enabled, draw additional debug information for the selected protozoa.
		if (snapshot.protozoa_tracker.is_active && snapshot.world_toggles.debug_mode)
		{
			draw_protozoa_debug(snapshot);
		}
	}


	void render_springs(const SimSnapshot& snapshot)
	{
		const float zoom = snapshot.sim_stats.camera_zoom;
		if (zoom < 0.012f || snapshot.cell_toggles.show_only_newborns)
			return;

		bool no_curves = zoom < 0.08f;
		connection_renderer_.update(snapshot.render.spring_connections, no_curves);
		connection_renderer_.draw(*m_window_);
	}

	void draw_protozoa_debug(const SimSnapshot& snapshot)
	{
		const OrganismTracker& protozoa = snapshot.protozoa_tracker;

		if (protozoa.is_active == false)
			return;

		if (snapshot.cell_toggles.skeleton_mode)
			draw_cell_outlines(protozoa);



		if (snapshot.cell_toggles.show_bounding_boxes)
			draw_protozoa_bounding_box(protozoa.bounds, *m_window_);

		draw_cell_physical_information(snapshot);
		draw_newborn_connection_radius(protozoa);
	}

	void draw_newborn_connection_radius(const OrganismTracker& protozoa)
	{
		sf::CircleShape circle_outline;
		circle_outline.setPointCount(30); // Reduce aliasing, set once
		circle_outline.setFillColor({ 0, 0, 0, 0 });
		circle_outline.setOutlineColor({ 255, 255, 255, 100 });
		circle_outline.setOutlineThickness(10.f);
		
		int i = 0;
		for (const Cell& cell : protozoa.cells)
		{
			if (cell.internal_clock_ >= CellManagerSettings::infant_time)
				continue;

			const Body& body = protozoa.bodies[i];
			const sf::Vector2f pos = body.position_;
			const float rad = cell.newborn_search_radius;

			circle_outline.setRadius(rad);
			circle_outline.setPosition(pos - sf::Vector2f{ rad, rad });

			m_window_->draw(circle_outline);
			i++;
		}
	}


	void draw_cell_outlines(const OrganismTracker& protozoa)
	{
		sf::CircleShape circle_outline;
		circle_outline.setPointCount(30); // Reduce aliasing, set once
		int i = 0;
		for (const Cell& cell : protozoa.cells)
		{
			const Body& body = protozoa.bodies[i];

			const sf::Vector2f pos = body.position_;
			const float rad = cell.radius + GraphicalSettings::cell_outline_thickness;

			circle_outline.setRadius(rad);
			circle_outline.setFillColor({ 0, 0, 0 });
			circle_outline.setPosition(pos - sf::Vector2f{ rad, rad });
			m_window_->draw(circle_outline);

			circle_outline.setFillColor({ 255, 0, 255 });
			circle_outline.setRadius(rad / 3);
			circle_outline.setPosition(pos - sf::Vector2f{ rad / 3, rad / 3 });
			m_window_->draw(circle_outline);

			i++;
		}
	}


	void draw_cell_physical_information(const SimSnapshot& snapshot) const
	{
		const OrganismTracker& protozoa = snapshot.protozoa_tracker;

		// for each cell we draw its bounding box
		int i = 0;
		for (const Cell& cell : protozoa.cells)
		{
			const sf::Vector2f& pos = protozoa.bodies[i].position_;
			const sf::Vector2f& vel = protozoa.bodies[i].velocity_;
			const float speed = vel.length();
			const float rad = cell.radius;

			// rendering the bounding boxes
			const sf::FloatRect rect = { {pos.x - rad, pos.y - rad}, {rad * 2, rad * 2} };
			draw_protozoa_bounding_box(rect, *m_window_);

			// drawing the direction of the cell
			const float arrow_length = std::min(rad * 4, speed * rad);
			draw_direction(*m_window_, pos, vel, arrow_length, 6, 10,
				{ 200, 220, 200 }, { 190, 200, 190 });

			// drawing cell stats
			const auto top_left = rect.position;
			//const auto spacing = font->get_text_size("0").y;
			//const sf::Vector2f offset = { 0, spacing };
			//font->draw(top_left, "id: " + std::to_string(cell.body_id_), false);

			i++;
		}
	}
};

// 319 lines