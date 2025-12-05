#include "world.h"
#include "../Utils/utility_SFML.h"
#include "../Utils/Graphics/buffer_renderer.h"

#include "../Utils/Graphics/spatial_hash_grid.h"

World::World(sf::RenderWindow* window)
	: m_window_(window),
	border_render_(make_circle(m_bounds_.radius, m_bounds_.center)),
	cell_grid_renderer(*window, world_bounds, cells_x, cells_y),
	food_grid_renderer(*window, world_bounds, FoodSettings::cells_x, FoodSettings::cells_y),
	thread_pool(8)
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
	handle_extinction_event();

	ticks++;
	update_cells_container();
	add_cells_to_hash_grid();

	if (toggle_collisions)
	{
		optimized_cell_collisions();
	}

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


void World::optimized_cell_collisions()
{
	// iterating over each cell, we update as per the cell instead of as per the particle
	for (int cell_id = 0; cell_id < cells_x * cells_y; ++cell_id)
	{
		update_grid_cell(cell_id);
	}
}

void World::update_grid_cell(const int grid_cell_id)
{
	/* All of the particles in this cell will be updated based on the information of the sorrounding 9 neighbours, this is a more optimized approach */ 
	int neighbours_size = 0;

	const int cell_index_x = grid_cell_id % cells_x;
	const int cell_index_y = grid_cell_id / cells_x;
	const bool at_border_x = cell_index_x == 0 || cell_index_x == cells_x - 1;
	const bool at_border_y = cell_index_y == 0 || cell_index_y == cells_y - 1;

	// each possible neighbour in the 3x3 area
	update_nearby_container(neighbours_size, cell_index_x - 1, cell_index_y - 1, true, true);
	update_nearby_container(neighbours_size, cell_index_x, cell_index_y - 1, false, true);
	update_nearby_container(neighbours_size, cell_index_x + 1, cell_index_y - 1, true, true);
	update_nearby_container(neighbours_size, cell_index_x - 1, cell_index_y, true, false);
	update_nearby_container(neighbours_size, cell_index_x, cell_index_y, false, false);
	update_nearby_container(neighbours_size, cell_index_x + 1, cell_index_y, true, false);
	update_nearby_container(neighbours_size, cell_index_x - 1, cell_index_y + 1, true, true);
	update_nearby_container(neighbours_size, cell_index_x, cell_index_y + 1, false, true);
	update_nearby_container(neighbours_size, cell_index_x + 1, cell_index_y + 1, true, true);

	// updating the particles
	const auto& cell_contents = spatial_hash_grid.grid[grid_cell_id];
	const uint8_t cell_size = spatial_hash_grid.objects_count[grid_cell_id];

//#pragma omp parallel for
	for (uint8_t idx = 0; idx < cell_size; ++idx)
	{
		update_protozoa_cell(cell_contents[idx], neighbours_size);
	}
}

void World::update_protozoa_cell(int protozoa_cell_index, int neighbours_size)
{
	Cell* cell = temp_cells_container[protozoa_cell_index];

	for (uint32_t i{ 0 }; i < neighbours_size; ++i)
	{
		Cell* other_cell = temp_cells_container[nearby_ids[i]];
		const sf::Vector2f other_pos = other_cell->position_;

		const float dist_sq = dist_squared(cell->position_, other_pos);
		const float local_diam = cell->radius * 2.f;

		if (dist_sq > local_diam * local_diam)
			continue;

		// Cells are not the same & they are intersecting
		const float dist = std::sqrt(dist_sq);
		if (dist == 0.0f) // Prevent division by zero
			continue;

		// Compute the overlap
		float overlap = local_diam - dist;

		// Compute the collision normal
		sf::Vector2f collisionNormal = (cell->position_ - other_pos) / dist;

		// Move the cells apart
		cell->position_ += collisionNormal * (overlap * 0.5f);
		other_cell->position_ -= collisionNormal * (overlap * 0.5f);

	}
}

void World::update_nearby_container(int& neighbours_size,
	int32_t neighbour_index_x, int32_t neighbour_index_y,
	bool check_x = true,
	bool check_y = true)
{
	// Fast modulo for positive and negative numbers
	if (check_x)
	{
		neighbour_index_x = neighbour_index_x >= 0 ?
			(neighbour_index_x < cells_x ? neighbour_index_x : neighbour_index_x - cells_x) :
			(neighbour_index_x + cells_x);
	}

	if (check_y)
	{
		neighbour_index_y = neighbour_index_y >= 0 ?
			(neighbour_index_y < cells_y ? neighbour_index_y : neighbour_index_y - cells_y) :
			(neighbour_index_y + cells_y);
	}

	// fetching data for copying
	const uint32_t neighbour_index = neighbour_index_y * cells_x + neighbour_index_x;
	const auto& contents = spatial_hash_grid.grid[neighbour_index];
	const auto size = spatial_hash_grid.objects_count[neighbour_index];

	// adding the neighbour data to the array
//#pragma omp parallel for
	for (uint8_t idx = 0; idx < size; ++idx)
	{
		nearby_ids[neighbours_size++] = contents[idx];
	}
}


void World::update_protozoas()
{
	std::vector<int> reproduce_indexes{};
	reproduce_indexes.reserve(max_protozoa);
	for (Protozoa* protozoa : all_protozoa)
	{
		protozoa->update(food_manager, debug_mode);

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

Protozoa* World::find_an_offspring()
{
	Protozoa* offspring = all_protozoa.add();

	if (offspring == nullptr)
	{
		const size_t idx = Random::rand_range(unsigned(0), all_protozoa.size() - 1);
		offspring = all_protozoa.at(idx);
	}
	return offspring;
}

void World::reproduce_protozoa(Protozoa* parent)
{
	Protozoa* offspring = find_an_offspring();
	parent->reproduce = false;
	
	// first we assign the genetic aspects of the offspring to match that of the parents, then reconstruct it
	offspring->reset_protozoa();
	offspring->set_protozoa_attributes(parent);
	offspring->generation += 1;
	offspring->mutate();
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


void World::handle_extinction_event()
{
	if (all_protozoa.size() == 0)
	{
		for (int i = 0; i < initial_protozoa; ++i)
		{
			Protozoa* protozoa = all_protozoa.add();
			protozoa->generate_random();
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
		spatial_hash_grid.add_object(cell->position_.x, cell->position_.y, idx++);
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
		selected_protozoa->render_debug(skeleton_mode, show_connections, show_bounding_boxes);
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
			outer_color_data.push_back(protozoa->cell_outer_color);
			inner_color_data.push_back(protozoa->cell_inner_color);
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