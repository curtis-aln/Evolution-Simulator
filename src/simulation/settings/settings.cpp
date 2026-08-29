#include "settings.h"
#include <toml++/toml.hpp>

#include "world/world_settings.h"
#include "managers/cell_manager/cell_manager_settings.h"
#include "managers/food_manager/food_manager_settings.h"
#include "entities/spring/spring_settings.h"
#include "entities/cell/cell_settings.h"

template<typename T>
static T require_value(const toml::node_view<toml::node>& node, std::string_view path)
{
    std::optional<T> val = node.value<T>();
    if (!val.has_value())
        throw std::runtime_error("Missing required config key: " + std::string(path));
    return *val;
}

#define REQUIRE(member, node) member = require_value<decltype(member)>(node, #node)

static void load_simulation_settings(toml::table& tbl)
{
    REQUIRE(SimulationSettings::full_screen, tbl["simulation"]["full_screen"]);
    REQUIRE(SimulationSettings::vsync, tbl["simulation"]["vsync"]);

    REQUIRE(SimulationSettings::initial_frame_rate_updating, tbl["simulation"]["max_fps_updating"]);
    REQUIRE(SimulationSettings::initial_frame_rate_rendering, tbl["simulation"]["max_fps_rendering"]);

	REQUIRE(SimulationSettings::max_iterations, tbl["simulation"]["max_iterations"]);
    REQUIRE(SimulationSettings::max_simulation_time, tbl["simulation"]["max_runtime"]);

    REQUIRE(SimulationSettings::ui_scale_percent, tbl["simulation"]["ui_scale_percent"]);
}

static void load_world_settings(toml::table& tbl)
{
    REQUIRE(WorldSettings::bounds_radius, tbl["world"]["bounds_radius"]);
    REQUIRE(WorldSettings::border_repulsion_magnitude, tbl["world"]["border_repulsion_magnitude"]);
    
	REQUIRE(WorldSettings::collision_grid_power, tbl["world"]["collision_grid_power"]);
	REQUIRE(WorldSettings::collision_max_capacity, tbl["world"]["collision_grid_max_capacity"]);

	REQUIRE(WorldSettings::birth_cell_power, tbl["world"]["birth_cell_power"]);
	REQUIRE(WorldSettings::birth_max_capacity, tbl["world"]["birth_grid_max_capacity"]);

	REQUIRE(WorldSettings::updating_threads, tbl["world"]["updating_threads"]);
	REQUIRE(WorldSettings::tick_sim_multiplier, tbl["world"]["tick_sim_multiplier"]);
}

static void load_cell_manager_settings(toml::table& tbl)
{
    REQUIRE(CellManagerSettings::max_cells, tbl["cell_manager"]["max_cells"]);
    REQUIRE(CellManagerSettings::initial_protozoa, tbl["cell_manager"]["initial_protozoa"]);
    REQUIRE(CellManagerSettings::auto_reset_on_extinction, tbl["cell_manager"]["auto_reset_on_extinction"]); 
	REQUIRE(CellManagerSettings::infant_time, tbl["cell_manager"]["infant_time"]);
	REQUIRE(CellManagerSettings::speed_energy_tax, tbl["cell_manager"]["speed_energy_tax"]);
}

static void load_cell_settings(toml::table& tbl)
{
    REQUIRE(CellSettings::spawn_radius, tbl["cell"]["spawn_radius"]);   
    REQUIRE(CellSettings::repro_cooldown, tbl["cell"]["reproductive_cooldown"]);
    REQUIRE(CellSettings::initial_energy, tbl["cell"]["initial_energy"]);
    REQUIRE(CellSettings::digestive_time, tbl["cell"]["digestive_time"]);
	REQUIRE(CellSettings::bite_amount, tbl["cell"]["bite_amount"]);

	REQUIRE(CellSettings::integrity_conversion_rate, tbl["cell"]["integrity_conversion_rate"]);
	REQUIRE(CellSettings::nutrients_conversion_rate, tbl["cell"]["nutrients_conversion_rate"]);

	REQUIRE(CellSettings::friction_energy_loss_const, tbl["cell"]["friction_energy_loss_const"]);
}

static void load_spring_settings(toml::table& tbl)
{
    REQUIRE(SpringSettings::spring_work_const, tbl["spring"]["spring_work_const"]);  
    REQUIRE(SpringSettings::maximum_extension, tbl["spring"]["maximum_extension"]);   
}

static void load_food_manager_settings(toml::table& tbl)
{
	REQUIRE(FoodManagerSettings::max_food, tbl["food_manager"]["max_food"]);
	REQUIRE(FoodManagerSettings::initial_food, tbl["food_manager"]["initial_food"]);
}


static void load_food_settings(toml::table& tbl)
{
    REQUIRE(FoodManagerSettings::food_radius, tbl["food"]["food_radius"]);
    REQUIRE(FoodManagerSettings::friction, tbl["food"]["friction"]);
    REQUIRE(FoodManagerSettings::death_age, tbl["food"]["death_age"]);
    REQUIRE(FoodManagerSettings::repro_cooldown, tbl["food"]["reproductive_cooldown"]);
    REQUIRE(FoodManagerSettings::nutrient_reproductive_threshold, tbl["food"]["nutrient_reproductive_threshold"]);
    REQUIRE(FoodManagerSettings::initial_nutrients, tbl["food"]["initial_nutrients"]);
    REQUIRE(FoodManagerSettings::final_nutrients, tbl["food"]["final_nutrients"]);
    REQUIRE(FoodManagerSettings::nutrient_development_time, tbl["food"]["nutrient_development_time"]);
    REQUIRE(FoodManagerSettings::food_spawn_distance, tbl["food"]["food_spawn_distance"]);
    REQUIRE(FoodManagerSettings::spawn_proportionality_constant, tbl["food"]["spawn_proportionality_constant"]);
    REQUIRE(FoodManagerSettings::death_age_chance, tbl["food"]["death_age_chance"]);
    REQUIRE(FoodManagerSettings::kFoodVisibilityRampFrames, tbl["food"]["food_visibility_ramp_frames"]);
    REQUIRE(FoodManagerSettings::kFoodMaxAlpha, tbl["food"]["food_max_alpha"]);
    REQUIRE(FoodManagerSettings::vibration_strength, tbl["food"]["vibration_strength"]);
    REQUIRE(FoodManagerSettings::update_freq, tbl["food"]["update_freq"]);
    REQUIRE(FoodManagerSettings::cell_max_capacity, tbl["food"]["cell_max_capacity"]);
    REQUIRE(FoodManagerSettings::cells_x, tbl["food"]["cells_x"]);
    REQUIRE(FoodManagerSettings::cells_y, tbl["food"]["cells_y"]);
}


void load_settings(const std::string& path)
{
    std::string full_path = std::string(PROJECT_ROOT) + "/" + path;

    toml::table tbl;

    try
    {
        tbl = toml::parse_file(path);
    }
    catch (const toml::parse_error& err)
    {
        std::cerr
            << "Error parsing file '" << err.source().path
            << "':\n" << err.description()
            << "\n (" << err.source().begin << ")\n";
        return;
    }

    std::cout << "Loading settings from: "
        << std::filesystem::absolute(path) << '\n';


    load_simulation_settings(tbl);
    load_world_settings(tbl);
    load_cell_settings(tbl);
	load_spring_settings(tbl);
    load_food_settings(tbl);
    load_cell_manager_settings(tbl);
    load_food_manager_settings(tbl);
}