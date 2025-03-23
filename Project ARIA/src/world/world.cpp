#include "world.h"
#include "../Utils/utility_SFML.h"
#include "../Utils/Graphics/buffer_renderer.h"

#include "../Utils/Graphics/spatial_hash_grid.h"

World::World(sf::RenderWindow* window)
	: m_window_(window),
	border_render_(make_circle(m_bounds_.radius, m_bounds_.center)),
	cell_grid_renderer(*window, world_bounds, cells_x, cells_y),
	food_grid_renderer(*window, world_bounds, FoodSettings::cells_x, FoodSettings::cells_y)
{ 
	init_organisms();
	init_food();
	init_environment();

	temp_cells_container.reserve(max_protozoa * GeneSettings::cell_amount_range.y);

	const size_t protozoa_count = all_protozoa.size();
	const size_t predicted_cells = protozoa_count * GeneSettings::cell_amount_range.y;

	// reserving nessesary data
	outer_color_data.reserve(predicted_cells);
	inner_color_data.reserve(predicted_cells);
	position_data.reserve(predicted_cells);
}

void World::update_world(bool pause)
{
	ticks++;
	update_cells_container();
	add_cells_to_hash_grid();
	handle_cell_collisions();

	// if our selected protozoa has died we can no longer track it
	if (selected_protozoa != nullptr && selected_protozoa->dead)
	{
		selected_protozoa = nullptr;
	}

	if (!pause)
	{ 
		food_manager.update();
		update_protozoas();
	}
}

void World::update_protozoas()
{
	std::vector<int> reproduce_indexes{};
	reproduce_indexes.reserve(max_protozoa);
	for (Protozoa* protozoa : all_protozoa)
	{
		protozoa->update(food_manager);

		if (protozoa->reproduce)
		{
			reproduce_indexes.push_back(protozoa->id);
		}

		if (protozoa->dead)
		{
			all_protozoa.remove(protozoa);
		}
	}

	for (int idx : reproduce_indexes)
	{
		reproduce_protozoa(all_protozoa.at(idx));
	}
}

void World::reproduce_protozoa(Protozoa* parent)
{
	parent->reproduce = false;
	Protozoa* offspring = all_protozoa.add();

	if (offspring == nullptr)
	{
		return;
	}
	
	// first we assign the genetic aspects of the offspring to match that of the parents, then reconstruct it
	offspring->set_cells_and_springs(parent->get_cells(), parent->get_springs());
	offspring->frames_alive = 0;
	offspring->generation += 1;
	offspring->dead = false;
	offspring->mutate();

	for (Cell& cell : offspring->get_cells())
	{
		cell.protozoa_id = offspring->id;
	}
}

void World::update_cells_container()
{
	temp_cells_container.clear();

	for (Protozoa* protozoa : all_protozoa)
	{
		for (Cell& cell : protozoa->get_cells())
		{
			temp_cells_container.push_back(&cell);
		}
	}
}

void World::add_cells_to_hash_grid()
{
	spatial_hash_grid.clear();
	int idx = 0;
	for (Cell* cell : temp_cells_container)
	{
		cell->bound(m_bounds_);
		spatial_hash_grid.addAtom(cell->position_, idx++);
	}
}

void World::handle_cell_collisions()
{
	for (size_t i = 0; i < temp_cells_container.size(); ++i)
	{
		Cell* cell = temp_cells_container[i];
		c_Vec<spatial_hash_grid.max_nearby_capacity>& nearby = spatial_hash_grid.find(cell->position_);

		for (int j = 0; j < nearby.size; ++j)
		{
			if (nearby.at(j) <= i)  // Ensure each pair is processed only once
				continue;

			Cell* other_cell = temp_cells_container.at(nearby.at(j));

			if (cell == other_cell)
				continue;

			if (debug_mode)
			{
				Protozoa* protozoa = all_protozoa.at(cell->protozoa_id);
				protozoa->cell_positions_nearby.push_back(other_cell->position_);

				Protozoa* other_protozoa = all_protozoa.at(other_cell->protozoa_id);
				other_protozoa->cell_positions_nearby.push_back(cell->position_);
			}

			if (paused)
			{
				continue;
			}

			const float dist_sq = dist_squared(cell->position_, other_cell->position_);
			const float local_diam = cell->radius + other_cell->radius;

			if (dist_sq > local_diam * local_diam)
				continue;

			// Cells are not the same & they are intersecting
			const float dist = std::sqrt(dist_sq);
			if (dist == 0.0f) // Prevent division by zero
				continue;

			// Compute the overlap
			float overlap = local_diam - dist;

			// Compute the collision normal
			sf::Vector2f collisionNormal = (cell->position_ - other_cell->position_) / dist;

			// Move the cells apart
			cell->position_ += collisionNormal * (overlap * 0.5f);
			other_cell->position_ -= collisionNormal * (overlap * 0.5f);
		}
	}
}

void World::update_debug(const sf::Vector2f mouse_position)
{
	if (selected_protozoa != nullptr)
	{
		selected_protozoa->move_selected_cell(mouse_position);
	}
}


void World::render_world()
{
	// In order to render such a large amount of organisms, we use vertex arrays, first we need to fetch the data from all protozoa.
	if (draw_cell_grid)
	{
		cell_grid_renderer.draw();
	}

	if (draw_food_grid)
	{
		food_grid_renderer.draw();
	}

	update_position_data();
	render_protozoa();

	// drawing the world bounds
	m_window_->draw(border_render_);
}

void World::render_protozoa()
{
	const float radius_inner = CellSettings::cell_radius;
	const float radius_outer = radius_inner + CellSettings::cell_outline_thickness;
	const int size = position_data.size();

	outer_circle_buffer.init_texture(outer_color_data, radius_outer, size);

	// if simple mode is off then we init the inner circle texture
	if (!simple_mode)
	{
		inner_circle_buffer.init_texture(inner_color_data, radius_inner, size);
	}

	// we want the springs to be rendered behind the cells
	if (selected_protozoa != nullptr && debug_mode && !skeleton_mode)
	{
		selected_protozoa->render_protozoa_springs();
	}

	// rendering the rest of the protozoas
	outer_circle_buffer.render(position_data);

	if (!simple_mode)
	{
		inner_circle_buffer.render(position_data);
	}

	// if our selected cell needs debugging
	if (debug_mode && selected_protozoa != nullptr)
	{
		selected_protozoa->render_debug(skeleton_mode);
	}

	// clearing all of their nearby data to be re-written to
	for (Protozoa* protozoa : all_protozoa)
	{
		protozoa->cell_positions_nearby.clear();
	}
}

void World::update_position_data()
{
	outer_color_data.clear();
	inner_color_data.clear();
	position_data.clear();

	food_manager.render();

	for (Protozoa* protozoa : all_protozoa)
	{
		for (Cell& cell : protozoa->get_cells())
		{
			outer_color_data.push_back(cell.outer_color);
			inner_color_data.push_back(cell.inner_color);
			position_data.push_back(cell.position_);
		}
	}
}


void World::init_organisms()
{
	for (int i = 0; i < max_protozoa; ++i)
	{
		all_protozoa.emplace({ i, &m_bounds_, m_window_, true });
	}
	for (int i = 0; i < max_protozoa - initial_protozoa; ++i)
	{
		all_protozoa.remove(i);
	}
}


void World::init_food()
{

}

void World::init_environment()
{

}

void World::check_hovering(const bool debug_mode, const sf::Vector2f mouse_position, bool mouse_pressed)
{
	if (!debug_mode)
		return;


	for (Protozoa* protozoa : all_protozoa)
	{
		if (protozoa->is_hovered_on(mouse_position))
		{
			break; // todo
		}
	}
}

bool World::check_pressed(const sf::Vector2f mouse_position)
{
	for (Protozoa* protozoa : all_protozoa)
	{
		if (protozoa->is_hovered_on(mouse_position, true))
		{
			selected_protozoa = protozoa;
			return true;
		}
	}
	return false;
}

void World::de_select_protozoa()
{
	if (selected_protozoa != nullptr)
	{
		//selected_protozoa->deselect_cell();
		//selected_protozoa = nullptr;
	}
}


float World::calculate_average_generation()
{
	float sum = 0.f;
	for (Protozoa* protozoa : all_protozoa)
	{
		sum += protozoa->generation;
	}
	return sum / all_protozoa.size();
}