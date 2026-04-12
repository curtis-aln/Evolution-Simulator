#include "settings.h"
#include <toml++/toml.hpp>

void load_settings(const std::string& path)
{
    std::string full_path = std::string(PROJECT_ROOT) + "/" + path;
   
    toml::table tbl = toml::parse_file(path);

    WorldSettings::bounds_radius = tbl["world"]["bounds_radius"].value_or(100000.f);
    WorldSettings::max_protozoa = tbl["world"]["max_protozoa"].value_or(50000u);
    WorldSettings::initial_protozoa = tbl["world"]["initial_protozoa"].value_or(30000u);
    WorldSettings::border_repulsion_magnitude = tbl["world"]["border_repulsion_magnitude"].value_or(0.001f);
    WorldSettings::max_speed = tbl["world"]["max_speed"].value_or(30.f);

    ProtozoaSettings::max_cells = tbl["protozoa"]["max_cells"].value_or(15);
    ProtozoaSettings::spawn_radius = tbl["protozoa"]["spawn_radius"].value_or(100.f);
    ProtozoaSettings::energy_decay_rate = tbl["protozoa"]["energy_decay_rate"].value_or(0.15f);
    ProtozoaSettings::reproductive_cooldown = tbl["protozoa"]["reproductive_cooldown"].value_or(200u);
    ProtozoaSettings::initial_energy = tbl["protozoa"]["initial_energy"].value_or(300.f);

    FoodSettings::food_radius = tbl["food"]["food_radius"].value_or(30.f);
    FoodSettings::friction = tbl["food"]["friction"].value_or(0.985f);
    FoodSettings::death_age = tbl["food"]["death_age"].value_or(700.f);
    FoodSettings::reproductive_cooldown = tbl["food"]["reproductive_cooldown"].value_or(150u);
}