#include "settings.h"
#include <toml++/toml.hpp>

#include "Protozoa/Protozoa.h"

void load_settings(const std::string& path)
{
    std::string full_path = std::string(PROJECT_ROOT) + "/" + path;

    toml::table tbl = toml::parse_file(path);

	SimulationSettings::full_screen = tbl["simulation"]["full_screen"].value_or(false);
	SimulationSettings::vsync = tbl["simulation"]["vsync"].value_or(true);
	SimulationSettings::max_fps = tbl["simulation"]["max_fps"].value_or(60u);
	SimulationSettings::ui_scale_percent = tbl["simulation"]["ui_scale_percent"].value_or(100.f);

	GraphicalSettings::spring_outline_thickness = tbl["graphical"]["spring_outline_thickness"].value_or(0.5f);
	GraphicalSettings::spring_thickness = tbl["graphical"]["spring_thickness"].value_or(1.f);
	GraphicalSettings::cell_outline_thickness = tbl["graphical"]["cell_outline_thickness"].value_or(1.f);
	GraphicalSettings::food_transparency = tbl["graphical"]["food_transparency"].value_or(128u);

    WorldSettings::bounds_radius = tbl["world"]["bounds_radius"].value_or(100000.f);
    WorldSettings::max_protozoa = tbl["world"]["max_protozoa"].value_or(50000u);
    WorldSettings::initial_protozoa = tbl["world"]["initial_protozoa"].value_or(30000u);
    WorldSettings::border_repulsion_magnitude = tbl["world"]["border_repulsion_magnitude"].value_or(0.001f);
    WorldSettings::max_speed = tbl["world"]["max_speed"].value_or(30.f);
	WorldSettings::cells_x = tbl["world"]["cells_x"].value_or(100u);
	WorldSettings::cells_y = tbl["world"]["cells_y"].value_or(100u);
	WorldSettings::cell_max_capacity = tbl["world"]["cell_max_capacity"].value_or(10u);

    ProtozoaSettings::max_cells = tbl["protozoa"]["max_cells"].value_or(15);
    ProtozoaSettings::spawn_radius = tbl["protozoa"]["spawn_radius"].value_or(100.f);
    ProtozoaSettings::energy_decay_rate = tbl["protozoa"]["energy_decay_rate"].value_or(0.15f);
    ProtozoaSettings::reproductive_cooldown = tbl["protozoa"]["reproductive_cooldown"].value_or(200u);
    ProtozoaSettings::initial_energy = tbl["protozoa"]["initial_energy"].value_or(300.f);
    ProtozoaSettings::wander_threshold = tbl["protozoa"]["wander_threshold"].value_or(200.f);
    ProtozoaSettings::spring_work_const = tbl["protozoa"]["spring_work_const"].value_or(0.0005f);
    ProtozoaSettings::breaking_length = tbl["protozoa"]["breaking_length"].value_or(90 * 6.f);
    ProtozoaSettings::maximum_extension = tbl["protozoa"]["maximum_extension"].value_or(90 * 4.f);
    ProtozoaSettings::digestive_time = tbl["protozoa"]["digestive_time"].value_or(100.f);
	ProtozoaSettings::cell_max_capacity = tbl["protozoa"]["cell_max_capacity"].value_or(10u);

    FoodSettings::food_radius = tbl["food"]["food_radius"].value_or(30.f);
    FoodSettings::friction = tbl["food"]["friction"].value_or(0.985f);
    FoodSettings::death_age = tbl["food"]["death_age"].value_or(700.f);
    FoodSettings::reproductive_cooldown = tbl["food"]["reproductive_cooldown"].value_or(150u);
    FoodSettings::reproductive_threshold = tbl["food"]["reproductive_threshold"].value_or(300.f);
    FoodSettings::initial_nutrients = tbl["food"]["initial_nutrients"].value_or(5.f);
    FoodSettings::final_nutrients = tbl["food"]["final_nutrients"].value_or(60.f);
    FoodSettings::nutrient_development_time = tbl["food"]["nutrient_development_time"].value_or(400u);
    FoodSettings::food_spawn_distance = tbl["food"]["food_spawn_distance"].value_or(50.f);
    FoodSettings::spawn_proportionality_constant = tbl["food"]["spawn_proportionality_constant"].value_or(0.005f);
    FoodSettings::death_age_chance = tbl["food"]["death_age_chance"].value_or(0.01f);
    FoodSettings::kFoodVisibilityRampFrames = tbl["food"]["kFoodVisibilityRampFrames"].value_or(100.f);
    FoodSettings::kFoodMaxAlpha = tbl["food"]["kFoodMaxAlpha"].value_or(0.8f);
    FoodSettings::vibration_strength = tbl["food"]["vibration_strength"].value_or(0.5f);
    FoodSettings::update_freq = tbl["food"]["update_freq"].value_or(5u);
    FoodSettings::initial_food = tbl["food"]["initial_food"].value_or(10000u);
    FoodSettings::max_food = tbl["food"]["max_food"].value_or(20000u);
	FoodSettings::cell_max_capacity = tbl["food"]["cell_max_capacity"].value_or(10u);
	FoodSettings::cells_x = tbl["food"]["cells_x"].value_or(100u);
	FoodSettings::cells_y = tbl["food"]["cells_y"].value_or(100u);
}
