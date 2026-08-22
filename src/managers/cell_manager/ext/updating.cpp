#include "../cell_manager.h"

void CellManager::deselect_cell()
{
	selected_cell_id_ = -1;
	protozoa_tracker_.is_active = false;
}


void CellManager::add_new_cells_to_grid()
{
	new_born_cell_grid_.clear();

	for (Cell* cell : all_cells_)
	{
		if (cell->internal_clock_ < infant_time)
		{
			Body* body = bodies_->at(cell->body_id_);
			new_born_cell_grid_.add_object(body->position_.x, body->position_.y, cell->id_);
		}
	}
}


void CellManager::update(int iterations)
{
	// if we have a selected cell and it has died, we need to deselect it to avoid null errors
	if (selected_cell_id_ == -1 || !all_cells_.at(selected_cell_id_)->is_alive())
	{
		deselect_cell();
	}

	// updating the cells and springs
	if (iterations > infant_time)
	{
		add_new_cells_to_grid();
	
	}
	update_springs();
	update_cells();
	
	update_cell_matter();

	// death
	collect_cell_death_requests();
	collect_matter_death_requests();

	apply_cell_death_requests();
	apply_matter_death_requests();
	
	// reproductive system
	collect_connection_requests(); // Connection Requests are collected here, but not applied yet
	apply_connection_requests();
	
	collect_reproduction_requests();
	apply_reproduction_requests();

	apply_matter_birth_requests();

	update_statistics();
}

void CellManager::update_cells()
{
	current_total_cells_ = all_cells_.occupied_count;
	ensure_update_jobs_built();
	thread_pool_.run_and_wait();
}

void CellManager::update_cell_matter()
{
	for (CellMatter* matter : all_cell_matter_)
	{
		Body* body = bodies_->at(matter->body_id_);
		matter->update(body);
	}
}

void CellManager::update_cell(Cell* cell)
{
	Body* body = bodies_->at(cell->body_id_);
	cell->update_statistics();
	cell->update_organics();
	body->velocity_ *= cell->sinwave_current_friction_;

	speed_tax_cell(cell);
	impulse_tax_cell(cell, body->impulse_);
}

void CellManager::impulse_tax_cell(Cell* cell, const float impulse)
{
	constexpr float impulse_damage_thresh = 15.5f;
	constexpr float impulse_damage_multiplier = 0.085f;

	if (impulse > impulse_damage_thresh)
	{
		float damage = (impulse - impulse_damage_thresh) * impulse_damage_multiplier;
		cell->integrity = std::max(0.f, cell->integrity - damage);
	}
}

void CellManager::collect_connection_requests()
{
	for (Cell* cell : all_cells_)
	{
		if (cell->internal_clock_ < infant_time && cell->internal_clock_ % infant_check_interval == 0)
			try_connect_newborn_cell(cell);
	}
}


void CellManager::update_position_container(RenderData& rend_data, const sf::FloatRect& visible_bounds, const bool show_only_newborns)
{
	int current_vector_size = rend_data.positions.size();
	int cell_count = get_cell_count();
	int resize_to = current_vector_size + cell_count;

	for (const Cell* cell : all_cells_)
	{
		Body* body = bodies_->at(cell->body_id_);
		if (!visible_bounds.contains(body->position_))
			continue;

		if (show_only_newborns && cell->internal_clock_ >= infant_time)
			continue;

		rend_data.outer_colors.push_back(cell->get_outer_color());
		rend_data.inner_colors.push_back(cell->get_inner_color());
		rend_data.positions.push_back(body->position_);
		rend_data.velocities.push_back(body->velocity_);
		rend_data.radii.push_back(cell->radius);
	}

	const int n = get_cell_count();

	// now we handle springs, we can just store the indexes as then the renderer can read them from the positions container above
	const int spring_count = static_cast<int>(all_springs_.size());
	rend_data.spring_connections.clear();

	for (Spring* spring : all_springs_)
	{
		Cell* cell_a = all_cells_.at(spring->cell_A_id);
		Body* body_a = bodies_->at(cell_a->body_id_);
		bool cell_a_visible = visible_bounds.contains({ body_a->position_.x, body_a->position_.y });

		if (!cell_a_visible || (show_only_newborns && cell_a->internal_clock_ > infant_time)) 
			continue;

		Cell* cell_b = all_cells_.at(spring->cell_B_id);
		Body* body_b = bodies_->at(cell_b->body_id_);
		bool cell_b_visible = visible_bounds.contains({ body_b->position_.x, body_b->position_.y });

		if (!cell_b_visible || (show_only_newborns && cell_b->internal_clock_ > infant_time)) 
			continue;

		const float min_dist = body_a->radius_ + body_b->radius_;

		// The reason why we cant use indexing to fill this array is because we dont know how many bodies are not active,
		// so it messes with the indexing and leads to null connections
		rend_data.spring_connections.push_back({
			get_cell_pos(spring->cell_A_id),
			get_cell_pos(spring->cell_B_id),
			min_dist,
			min_dist * 2.f,
			spring->stress });
	}

	if (show_only_newborns)
		return;

	for (const CellMatter* matter : all_cell_matter_)
	{
		Body* body = bodies_->at(matter->body_id_);

		if (!visible_bounds.contains(body->position_))
			continue;

		rend_data.outer_colors.push_back(matter->outer_color());
		rend_data.inner_colors.push_back(matter->inner_color());
		rend_data.positions.push_back(body->position_);
		rend_data.velocities.push_back(body->velocity_);
		rend_data.radii.push_back(body->radius_);
	}
}

void CellManager::try_connect_newborn_cell(Cell* cell)
{
	Body* body = bodies_->at(cell->body_id_);

	// this cell is going to try to connect to other cells in its vicinity
	nearby_cell_ids.clear();
	new_born_cell_grid_.find(body->position_.x, body->position_.y, &nearby_cell_ids);

	for (int i = 0; i < nearby_cell_ids.count; ++i)
	{
		cell_idx other_cell_id = nearby_cell_ids[i];
		if (other_cell_id == cell->id_)
			continue;

		Cell* other_cell = all_cells_.at(other_cell_id);

		float dist_sq = (body->position_ - bodies_->at(other_cell->body_id_)->position_).lengthSquared();
		if (dist_sq < connection_range * connection_range)
		{
			connection_requests.push_back(ConnectionRequest{ (int32_t)cell->id_, (int32_t)other_cell->id_ });
		}
	}
}

void CellManager::update_springs()
{
	std::vector<int> to_remove;
	for (Spring* spring : all_springs_)
	{
		// if the spring has broken on its own
		if (spring->is_spring_broken())
		{
			to_remove.push_back(spring->id_);
			continue;
		}

		Cell* cell_a = all_cells_.at(spring->cell_A_id);
		Cell* cell_b = all_cells_.at(spring->cell_B_id);

		// otherwise we update the spring physics and organics
		Body* body_a = bodies_->at(cell_a->body_id_);
		Body* body_b = bodies_->at(cell_b->body_id_);

		spring->update_physics(body_a->position_, body_a->velocity_, body_b->position_, body_b->velocity_);
		body_a->accelerate(spring->movement_vector);
		body_b->accelerate(-spring->movement_vector);
		spring->update_organics(*cell_a, *cell_b);
	}

	for (int spring_id : to_remove)
	{
		all_springs_.remove(spring_id);
	}
}


void CellManager::check_for_extinction_event()
{
	// if protozoas are still alivee or if auto reset on extinction is disabled, we dont need to do anything
	if (all_cells_.size() > 0 || !auto_reset_on_extinction)
		return;

	std::cout << "Extinction event occurred, respawning initial protozoa...\n";

	//for (unsigned i = 0; i < initial_protozoa; ++i)
	//{
	//	Protozoa* protozoa = all_protozoa_.add();
	//	build_protozoa(*protozoa, world_bounds, false);
	//}
}


Cell* CellManager::find_cell_at_point(const sf::Vector2f mouse_position, bool make_selected_cell)
{
	// When this function is called it checks every cell in the world to see if the mouse click is within the bounds of any cell. 
	// If it finds a cell that contains the mouse position, it sets that cell as the selected cell and returns true. 
	//  bool make_selected_cell - If true, the cell that is found will be set as the selected cell.
	protozoa_tracker_.is_active = false;
	for (Cell* cell : all_cells_)
	{
		Body* body = bodies_->at(cell->body_id_);
		float dist_sq = (mouse_position - body->position_).lengthSquared();
		if (dist_sq < cell->radius * cell->radius)
		{
			// We tell the cell manager which cell is selected, 
			// so it can be used in other parts of the program (like rendering debug info for the selected cell).
			if (make_selected_cell)
			{
				selected_cell_id_ = cell->id_;
				protozoa_tracker_.is_active = true;
			}
			return cell;
		}
	}
	return nullptr;
}

void CellManager::fill_snapshot(SimSnapshot& snapshot, sf::FloatRect& visible_bounds)
{
	snapshot.protozoa_tracker = protozoa_tracker_;

	snapshot.world_stats.highlighted_cells = select_indexes.count;
}


const sf::Vector2f* CellManager::get_selected_protozoa_pos() const
{
	// This function returns the position of the selected protozoa, 
	// if there is one.
	if (selected_cell_id_ != -1)
	{
		Body* body = bodies_->at(all_cells_.at(selected_cell_id_)->body_id_);
		return &body->position_;
	}
	return nullptr;
}

sf::Vector2f& CellManager::get_cell_pos(int cell_id)
{
	return bodies_->at(all_cells_.at(cell_id)->body_id_)->position_;
}

Body* CellManager::get_cell_body(int cell_id)
{
	return bodies_->at(all_cells_.at(cell_id)->body_id_);
}

void CellManager::drag_selected_cell_to_point(const sf::Vector2f& target_position, const float move_fraction)
{
	if (selected_cell_id_ == -1)
		return;

	Body* body = bodies_->at(all_cells_.at(selected_cell_id_)->body_id_);
	body->position_ = target_position;
	
	const sf::Vector2f mouse_pos = m_window_->mapPixelToCoords(sf::Mouse::getPosition(*m_window_));
	const sf::Vector2f diff = mouse_pos - body->position_;
	body->position_ += diff * move_fraction; // apply a small force towards the mouse position
}


CellBodyPair CellManager::create_cell(sf::Vector2f position, bool random_genetics)
{
	// This is the safest way to create a cell with a body, all creation events Must go through this function to ensure that the cell and body are linked correctly.
	// if there are not any already avalable cells in the o_vector we create a new one

	// Finding a body
	Body* body = bodies_->emplace(true, true);
	if (body == nullptr)
		return { .is_valid = false };

	// Finding a cell
	Cell* cell = all_cells_.emplace(true, true);
	if (cell == nullptr)
	{
		// raise an error as there shouldnt be a situation where we have a body but no cell, this should never happen
		std::cerr << "[ERROR]: Failed to create cell during initialization. Max cells reached.\n";
		bodies_->remove(body);
		return { .is_valid = false };
	}

	// resetting the cell just incase it isnt brand new
	cell->recreate();

	// resetting the body just incase it isnt brand new
	body->reset_cell_manager();

	// connecting the two
	cell->body_id_ = body->id_;
	body->position_ = position;

	if (random_genetics)
	{
		cell->randomize();
	}

	body->radius_ = cell->radius;
	body->mass_ = body->radius_;

	register_birth_stat();

	return { cell->id_, body->id_, true };
}

int32_t CellManager::create_spring(const uint32_t cell_a_id, const uint32_t cell_b_id)
{
	Spring* spring = all_springs_.emplace(true, true);
	
	if (spring == nullptr)
	{
		return -1;
	}
	spring->reset_cell_manager();
	spring->cell_A_id = cell_a_id;
	spring->cell_B_id = cell_b_id;
	return spring->id_;
}
