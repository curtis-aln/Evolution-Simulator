#include "../cell_manager.h"

void CellManager::gather_food_in_radius(FixedSpan<cell_idx, uint16_t>& indexes, const sf::Vector2f& position, const float radius)
{
	indexes.clear();

	for (Cell* cell : all_cells_)
	{
		Body* body = bodies_->at(cell->body_id_);
		float dist_sq = (body->position_ - position).lengthSquared();
		if (dist_sq < radius * radius)
		{
			indexes.add(cell->id_);
		}
	}
}

void CellManager::remove_cells_in_radius(const sf::Vector2f& position, const float radius)
{
	gather_food_in_radius(select_indexes, position, radius);

	for (int i = 0; i < select_indexes.count; ++i)
		remove_cell(select_indexes[i]);

	for (Spring* spring : all_springs_)
	{
		// making sure we remove the springs that are connected to the cells that have been removed
		Cell* cell_a = all_cells_.at(spring->cell_A_id);
		Cell* cell_b = all_cells_.at(spring->cell_B_id);
		if (all_cells_.is_obj_active(cell_a->id_) == false || all_cells_.is_obj_active(cell_b->id_) == false)
		{
			all_springs_.remove(spring);
		}
	}
}

void CellManager::influence_cell_velocities_in_radii(const sf::Vector2f& position, const float radius, const int intensity)
{
	gather_food_in_radius(select_indexes, position, radius);

	for (int i = 0; i < select_indexes.count; ++i)
	{
		Cell* cell = all_cells_.at(select_indexes[i]);
		Body* body = bodies_->at(cell->body_id_);

		sf::Vector2f direction = (position - body->position_).normalized();
		body->velocity_ += direction * (float)intensity;
	}
}

void CellManager::handle_cell_manager_event(SimCommand& cmd)
{
	switch (cmd.type)
	{
	case CommandType::SetCellToggles:
		toggles_ = cmd.cell_toggles;
		break;

		// Spring Natual Selection Modifier
	case CommandType::SetSpringBreakingForce:
		Spring::SPRING_BREAK_FORCE = cmd.float_val;
		break;
	case CommandType::SetSpringBreakingLength:
		Spring::SPRING_BREAK_LENGTH = cmd.float_val;
		break;
	case CommandType::SetSpringDamageThreshold:
		Spring::SPRING_DAMAGE_THRESH = cmd.float_val;
		break;
	case CommandType::SetSpringWorkConst:
		Spring::SPRING_WORK_CONST = cmd.float_val;
		break;

	case CommandType::SetMinSpeed:
		statistics_.min_speed = cmd.float_val;
		break;

	case CommandType::SetRadius:
		//    if (selected_protozoa)
		//        selected_protozoa->get_cells()[cmd.cell_spring_idx].radius = cmd.float_val;
		break;

	case CommandType::SetAmplitude:
		//    if (selected_protozoa)
		//        selected_protozoa->get_cells()[cmd.cell_spring_idx].amplitude = cmd.float_val;
		break;

	case CommandType::SetFrequency:
		//if (selected_protozoa)
		//    selected_protozoa->get_cells()[cmd.cell_spring_idx].frequency = cmd.float_val;
		break;

	case CommandType::SetVerticalShift:
		//if (selected_protozoa)
		//    selected_protozoa->get_cells()[cmd.cell_spring_idx].vertical_shift = cmd.float_val;
		break;

	case CommandType::SetOffset:
		//if (selected_protozoa)
		//    selected_protozoa->get_cells()[cmd.cell_spring_idx].offset = cmd.float_val;
		break;

	case CommandType::MutateProtozoa:
		//if (selected_protozoa)
	   //     selected_protozoa->mutate(cmd.mutate.mut_rate, cmd.mutate.mut_range);
		break;

	case CommandType::AddCell:
		//if (selected_protozoa)
		//    selected_protozoa->add_cell();
		break;

	case CommandType::RemoveCell:
		//if (selected_protozoa)
		//    selected_protozoa->remove_cell();
		break;

	case CommandType::AddSpring:
		//if (selected_protozoa)
		//    selected_protozoa->add_spring(); todo
		break;

	case CommandType::RemoveSpring:
		//if (selected_protozoa)
		//    selected_protozoa->remove_spring(); todo
		break;

	case CommandType::InjectProtozoa:
		//if (selected_protozoa)
		//    m_world_.inject_protozoa(selected_protozoa, cmd.float_val);
		break;

	case CommandType::KillProtozoa:
		kill_selected_protozoa();
		break;

	case CommandType::ForceReproduce:
		force_reproduce_selected_protozoa();
		break;

	case CommandType::MakeImmortal:
		//if (selected_protozoa) // todo
		//    selected_protozoa->immortal = cmd.bool_val;
		break;

	case CommandType::CloneProtozoa:
		clone_selected_protozoa();
		break;

	case CommandType::SetSpringAmplitude:
		//if (selected_protozoa)
		//{
		//    m_world_.selected_protozoa_->get_springs()[cmd.cell_spring_idx].amplitude = cmd.float_val;
		//}
		break;

	case CommandType::SetSpringFrequency:
		//if (selected_protozoa)
		//{
		//    m_world_.selected_protozoa_->get_springs()[cmd.cell_spring_idx].frequency = cmd.float_val;
		//}
		break;

	case CommandType::SetSpringOffset:
		//if (selected_protozoa)
		//{
		//    m_world_.selected_protozoa_->get_springs()[cmd.cell_spring_idx].offset = cmd.float_val;
		//}
		//break;
			//m_world_.selected_protozoa_->get_springs()[cmd.cell_spring_idx].offset = cmd.float_val;
		//}
		break;

	case CommandType::SetSpringVerticalShift:
		//if (selected_protozoa)
		//{
			//m_world_.selected_protozoa_->get_springs()[cmd.cell_spring_idx].vertical_shift = cmd.float_val;
		//}
		break;

	case CommandType::SetDampingConst:
		//if (selected_protozoa)
		//{
		   // m_world_.selected_protozoa_->get_springs()[cmd.cell_spring_idx].damping = cmd.float_val;
		//}
		break;

	case CommandType::SetSpringConst:
		//if (selected_protozoa)
		//{
		   // m_world_.selected_protozoa_->get_springs()[cmd.cell_spring_idx].spring_const = cmd.float_val;
		//}
		break;

	}
}

void CellManager::kill_selected_protozoa()
{
	// every cell and spring in the current selected protozoa is removed from the world
	if (selected_cell_id_ == -1 || protozoa_tracker_.is_active == false)
		return;

	for (Cell& cell : protozoa_tracker_.cells)
	{
		remove_cell(cell.id_);
	}

	for (Spring& spring : protozoa_tracker_.springs)
	{
		all_springs_.remove(spring.id_);
	}
}

void CellManager::force_reproduce_selected_protozoa()
{
	// every cell in the current selected protozoa is forced to reproduce by making their nutrients, energy, and integrity reach
	// their minimum reproduction thresholds
	if (selected_cell_id_ == -1 || protozoa_tracker_.is_active == false)
		return;

	for (Cell& fake_cell : protozoa_tracker_.cells)
	{
		Cell* cell = all_cells_.at(fake_cell.id_);
		cell->nutrients_ = std::max(cell->nutrients_, cell->birth_nutrients_thresh * CellSettings::max_nutrients);
		
		cell->set_energy(std::max(cell->get_energy(), cell->birth_energy_thresh * CellSettings::max_energy));
		cell->set_integrity(std::max(cell->get_integrity(), cell->birth_integrity_thresh * CellSettings::max_integrity));
		
		cell->repro_timer_ = std::max(cell->repro_timer_, uint16_t(cell->repro_cooldown));
		cell->force_reproduce();
	}
}


void CellManager::clone_selected_protozoa()
{
	if (selected_cell_id_ == -1 || protozoa_tracker_.is_active == false)
		return;

	sf::Vector2f parent_center = protozoa_tracker_.position;
	sf::Vector2f child_spawn_pos = Random::rand_position_in_circle(parent_center, 100.f);
	float child_spawn_rad = 60.f;

	// maps the parent's absolute cell id -> the corresponding clone's absolute cell id
	std::unordered_map<int32_t, int32_t> old_to_new_cell_id;
	old_to_new_cell_id.reserve(protozoa_tracker_.cells.size());

	for (Cell& cell : protozoa_tracker_.cells)
	{

		CellBodyPair cpair = create_cell({ 0, 0 }, false);

		if (!cpair.is_valid)
		{
			std::cerr << "[ERROR]: Failed to create cell during protozoa cloning. Max cells reached.\n";
			return;
		}

		const Body* parent_body = bodies_->at(cell.body_id_);

		Cell* new_cell = all_cells_.at(cpair.cell_id);
		Body* new_body = bodies_->at(cpair.body_id);

		new_cell->copy_genetics(cell);
		new_body->copy(parent_body);
		new_body->position_ = Random::rand_position_in_circle(child_spawn_pos, child_spawn_rad);

		old_to_new_cell_id[cell.id_] = cpair.cell_id;
	}

	for (Spring& spring : protozoa_tracker_.springs)
	{
		auto it_a = old_to_new_cell_id.find(spring.cell_A_id);
		auto it_b = old_to_new_cell_id.find(spring.cell_B_id);

		if (it_a == old_to_new_cell_id.end() || it_b == old_to_new_cell_id.end())
		{
			std::cerr << "[ERROR]: Spring references a cell not present in the cloned set.\n";
			continue;
		}

		int32_t spring_id = create_spring(it_a->second, it_b->second);

		if (spring_id == -1)
		{
			std::cerr << "[ERROR]: Failed to create spring during protozoa cloning. Max springs reached.\n";
			return;
		}

		Spring* new_spring = all_springs_.at(spring_id);

		new_spring->genome.copy_genetics(spring.genome);
	}
}