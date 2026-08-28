#include "../food_manager.h"
#include "../../../simulation/context/sim_snapshot.h"


FoodManager::FoodManager(sf::RenderWindow* window, WorldBorder* world_bounds, o_vector<Body>* bodies)
	: world_bounds_(world_bounds), bodies_(bodies),
	pheromone_grid(512, 512, WorldSettings::bounds_radius * 2.f, WorldSettings::bounds_radius * 2.f)
{

}


void FoodManager::update()
{
	food_reproduction_function();
	update_food();
	update_statistics();
	handle_food_death();
}

void FoodManager::handle_food_death()
{
	for (Food* food : food_vector)
	{
		if (food->is_food_dead())
			remove_food(food->id_);

		sf::Vector2f pos = bodies_->at(food->body_id_)->position_;

		if (toggles_.update_pheromone_grid)
			pheromone_grid.add_pheromone(pos.x, pos.y, 10.f);
	}

	if (toggles_.update_pheromone_grid)
		pheromone_grid.step();
}

void FoodManager::update_position_data(RenderData& food_data)
{
	int current_vector_size = food_data.positions.size();
	int food_count = food_vector.size();
	int reserve_to = current_vector_size + food_count;

	food_data.positions.reserve(reserve_to);
	food_data.radii.reserve(reserve_to);
	food_data.inner_colors.reserve(reserve_to);
	food_data.outer_colors.reserve(reserve_to);
	food_data.velocities.reserve(reserve_to);

	if (toggles_.render_pheromone_grid)
	{
		pheromone_grid.render();
		food_data.pheromone_texture = pheromone_grid.get_texture();
	}

	for (Food* food : food_vector)
	{
		Body* body = bodies_->at(food->body_id_);

		food_data.positions.push_back(body->position_);
		food_data.radii.push_back(body->radius_);
		food_data.velocities.push_back(body->velocity_);

		// handling color
		sf::Color inner_col_copy = food->color_inner;
		sf::Color outer_col_copy = food->color_outer;
		set_foods_color_transparency(inner_col_copy, food->inner_transparency, food->nutrients, food->age);
		set_foods_color_transparency(outer_col_copy, food->outer_transparency, food->nutrients, food->age);
		food_data.inner_colors.push_back(inner_col_copy);
		food_data.outer_colors.push_back(outer_col_copy);

	}
}

void FoodManager::set_foods_color_transparency(sf::Color& color_to_change, 
	const float transparency, const float nutrients, const float age) const
{
	const bool is_dying = age >= death_age;

	if (!is_dying)
	{
		// Fade in over the first kFoodVisibilityRampFrames frames
		const float t = std::min(static_cast<float>(age) / kFoodVisibilityRampFrames, 1.f);
		color_to_change.a = static_cast<uint8_t>(t * transparency);
	}
	else
	{
		// Fade out as nutrients fall from fade_start_nutrients down to initial_nutrients
		const float range = fade_start_nutrients - initial_nutrients;
		const float t = std::clamp(
			(nutrients - initial_nutrients) / range,
			0.f, 1.f
		);
		color_to_change.a = static_cast<uint8_t>(t * transparency);
	}
}

// world interacting with the food
void FoodManager::remove_food(const int food_id)
{
	// Fetching the food that we want to remove and using its body_id to find the body we want to remove
	Food* food = food_vector.at(food_id);
	Body* body = bodies_->at(food->body_id_);
	
	// we now reset the body and the food
	body->position_ = { 0, 0 };
	food->age = 0;
	food->time_since_last_reproduced = 0;
	food->nutrients = initial_nutrients;
	
	// and remove them from their respective vectors
	food_vector.remove(food_id);
	bodies_->remove(body->id_);
}

Food* FoodManager::at(const int idx)
{
	return food_vector.at(idx);
}

const Food* FoodManager::at(const int idx) const
{
	return food_vector.at(idx);
}


int FoodManager::get_size() const
{
	return food_vector.size();
}


const o_vector<Food>& FoodManager::get_food_vector() const
{
	return food_vector;
}
o_vector<Food>& FoodManager::get_food_vector()
{
	return food_vector;
}


bool FoodManager::has_food_with_body_id(int body_id)
{
	for (Food* food : food_vector)
	{
		if (food->body_id_ == body_id)
			return true;
	}
	return false;
}

void FoodManager::handle_food_manager_event(SimCommand& cmd)
{
	switch (cmd.type)
	{
	case CommandType::SetFoodToggles:
		toggles_ = cmd.food_toggles;
		break;

	case CommandType::SetRandomIntensity:
		statistics_.food_random_spawn_intensity = cmd.int_val;
		break;

	case CommandType::SetMitosisIntensity:
		statistics_.food_mitosis_spawn_intensity = cmd.float_val;
		break;

	case CommandType::SetPheromoneUpdatingSteps:
		PheromoneGridSettings::substeps = cmd.int_val;
		break;

	case CommandType::SetPheromoneDecayRate:
		PheromoneGridSettings::decay_rate = cmd.float_val;
		break;

	case CommandType::SetPheromoneDiffusionRate:
		PheromoneGridSettings::diffuse_rate = cmd.float_val;
		break;

	case CommandType::SetPheromoneDepositAmount:
		PheromoneGridSettings::deposit_amount = cmd.float_val;
		break;

	case CommandType::SetPheromoneMaxPheromone:
		PheromoneGridSettings::max_pheromone = cmd.float_val;
		break;

	case CommandType::SetFoodNutrients:
		FoodManagerSettings::final_nutrients = cmd.float_val;
		break;

	case  CommandType::SetFoodNutrientsDevelopmentTime:
		FoodManagerSettings::nutrient_development_time = static_cast<size_t>(cmd.float_val);
		break;

	case  CommandType::SetFoodFriction:
		FoodManagerSettings::friction = cmd.float_val;
		break;

	case  CommandType::SetFoodVibrationStrength:
		FoodManagerSettings::vibration_strength = cmd.float_val;
		break;

	case CommandType::SetFoodReproductiveCooldown:
		FoodManagerSettings::repro_cooldown = static_cast<size_t>(cmd.float_val);
		break;

	case  CommandType::SetFoodDeathAge:
		FoodManagerSettings::death_age = cmd.float_val;
		break;

	case  CommandType::SetFoodReproductiveThreshold:
		FoodManagerSettings::reproductive_threshold = cmd.float_val;
		break;
	}
}