#include "settings.h"
#include <toml++/toml.hpp>

#include "Protozoa/Protozoa.h"

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
    REQUIRE(SimulationSettings::max_fps, tbl["simulation"]["max_fps"]);
    REQUIRE(SimulationSettings::ui_scale_percent, tbl["simulation"]["ui_scale_percent"]);
}

static void load_graphical_settings(toml::table& tbl)
{
    REQUIRE(GraphicalSettings::spring_outline_thickness, tbl["graphical"]["spring_outline_thickness"]);
    REQUIRE(GraphicalSettings::spring_thickness, tbl["graphical"]["spring_thickness"]);
    REQUIRE(GraphicalSettings::cell_outline_thickness, tbl["graphical"]["cell_outline_thickness"]);
    REQUIRE(GraphicalSettings::food_transparency, tbl["graphical"]["food_transparency"]);
}

static void load_world_settings(toml::table& tbl)
{
    REQUIRE(WorldSettings::bounds_radius, tbl["world"]["bounds_radius"]);
    REQUIRE(WorldSettings::max_protozoa, tbl["world"]["max_protozoa"]);
    REQUIRE(WorldSettings::initial_protozoa, tbl["world"]["initial_protozoa"]);
    REQUIRE(WorldSettings::border_repulsion_magnitude, tbl["world"]["border_repulsion_magnitude"]);
    REQUIRE(WorldSettings::max_speed, tbl["world"]["max_speed"]);
    REQUIRE(WorldSettings::cells_x, tbl["world"]["cells_x"]);
    REQUIRE(WorldSettings::cells_y, tbl["world"]["cells_y"]);
    REQUIRE(WorldSettings::cell_max_capacity, tbl["world"]["cell_max_capacity"]);
}

static void load_protozoa_settings(toml::table& tbl)
{
    REQUIRE(ProtozoaSettings::max_cells, tbl["protozoa"]["max_cells"]);
    REQUIRE(ProtozoaSettings::spawn_radius, tbl["protozoa"]["spawn_radius"]);
    REQUIRE(ProtozoaSettings::energy_decay_rate, tbl["protozoa"]["energy_decay_rate"]);
    REQUIRE(ProtozoaSettings::reproductive_cooldown, tbl["protozoa"]["reproductive_cooldown"]);
    REQUIRE(ProtozoaSettings::initial_energy, tbl["protozoa"]["initial_energy"]);
    REQUIRE(ProtozoaSettings::wander_threshold, tbl["protozoa"]["wander_threshold"]);
    REQUIRE(ProtozoaSettings::spring_work_const, tbl["protozoa"]["spring_work_const"]);
    REQUIRE(ProtozoaSettings::breaking_length, tbl["protozoa"]["breaking_length"]);
    REQUIRE(ProtozoaSettings::maximum_extension, tbl["protozoa"]["maximum_extension"]);
    REQUIRE(ProtozoaSettings::digestive_time, tbl["protozoa"]["digestive_time"]);
}

static void load_food_settings(toml::table& tbl)
{
    REQUIRE(FoodSettings::food_radius, tbl["food"]["food_radius"]);
    REQUIRE(FoodSettings::friction, tbl["food"]["friction"]);
    REQUIRE(FoodSettings::death_age, tbl["food"]["death_age"]);
    REQUIRE(FoodSettings::reproductive_cooldown, tbl["food"]["reproductive_cooldown"]);
    REQUIRE(FoodSettings::reproductive_threshold, tbl["food"]["reproductive_threshold"]);
    REQUIRE(FoodSettings::initial_nutrients, tbl["food"]["initial_nutrients"]);
    REQUIRE(FoodSettings::final_nutrients, tbl["food"]["final_nutrients"]);
    REQUIRE(FoodSettings::nutrient_development_time, tbl["food"]["nutrient_development_time"]);
    REQUIRE(FoodSettings::food_spawn_distance, tbl["food"]["food_spawn_distance"]);
    REQUIRE(FoodSettings::spawn_proportionality_constant, tbl["food"]["spawn_proportionality_constant"]);
    REQUIRE(FoodSettings::death_age_chance, tbl["food"]["death_age_chance"]);
    REQUIRE(FoodSettings::kFoodVisibilityRampFrames, tbl["food"]["food_visibility_ramp_frames"]);
    REQUIRE(FoodSettings::kFoodMaxAlpha, tbl["food"]["food_max_alpha"]);
    REQUIRE(FoodSettings::vibration_strength, tbl["food"]["vibration_strength"]);
    REQUIRE(FoodSettings::update_freq, tbl["food"]["update_freq"]);
    REQUIRE(FoodSettings::initial_food, tbl["food"]["initial_food"]);
    REQUIRE(FoodSettings::max_food, tbl["food"]["max_food"]);
    REQUIRE(FoodSettings::cell_max_capacity, tbl["food"]["cell_max_capacity"]);
    REQUIRE(FoodSettings::cells_x, tbl["food"]["cells_x"]);
    REQUIRE(FoodSettings::cells_y, tbl["food"]["cells_y"]);
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


    load_simulation_settings(tbl);
    load_graphical_settings(tbl);
    load_world_settings(tbl);
    load_protozoa_settings(tbl);
    load_food_settings(tbl);
}