#pragma once

#include <SFML/Graphics.hpp>

#include "../../entities/food/food.h"

#include "../../Utils/Graphics/CircleBatchRenderer.h"
#include "../../Utils/o_vec/o_vector.hpp"
#include "food_manager_settings.h"
#include "../../Utils/spatial_grid/simple_spatial_grid.h"
#include "../../Utils/spatial_grid/spatial_grid_renderer.h"
#include "world/world_border.h"
#include "../../simulation/context/state.h"
#include <Utils/thread_pool.h>

#include <world/world_settings.h>

#include "../../simulation/context/state.h"
#include <simulation/context/sim_command.h>


struct FoodBodyPair
{
    int32_t food_id;
    int32_t body_id;

    bool is_valid()
    {
        return (food_id != -1) && (body_id != -1);
    }
};


struct SimSnapshot;

class FoodManager : public FoodManagerSettings
{
    sf::RenderWindow* window_;
    WorldBorder* world_bounds_;

    o_vector<Body>* bodies_;
    o_vector<Food> food_vector{ max_food };
    
	FoodManagerStatistics statistics_{};

    std::vector<std::function<void()>> updating_bodies_;
    BarrierThreadPool thread_pool_{ (int)WorldSettings::updating_threads };
	bool update_jobs_built_ = false;
    int  current_total_food_ = 0;

public:
    FixedSpan<cell_idx, uint16_t> selected_cells_indexes_{ static_cast<uint16_t>(10000) };

    int frames = 0;

    FoodToggles toggles_{};

public:
    FoodManager(sf::RenderWindow* window, WorldBorder* world_bounds, o_vector<Body>* bodies);
    void  create_food(unsigned amount);

    void reset_food_manager();

    void remove_food_in_area(const sf::Vector2f& center, float radius);

    void gather_food_in_radius(FixedSpan<cell_idx, uint16_t>& indexes, const sf::Vector2f& position, const float radius);

    void influence_food_velocities_in_radii(const sf::Vector2f& position, const float radius, const int intensity);

    int    get_size()               const;
    bool has_food_with_body_id(int body_id);
    void handle_food_manager_event(SimCommand& cmd);
    const o_vector<Food>& get_food_vector() const;
    o_vector<Food>& get_food_vector();
    void update();

    void update_position_data(RenderData& food_data);
    void set_foods_color_transparency(sf::Color& color_to_change, const float transparency, const float nutrients, const float age) const;
    void   remove_food(int food_id);
    Food* at(int idx);
    const Food* at(int idx) const;

    FoodBodyPair create_food(const sf::Vector2f& position);

	FoodManagerStatistics& get_statistics() { return statistics_; }

private:
    void  update_food();
    void update_food_item(Food* food);
    void ensure_update_jobs_built();
    void update_statistics();
    
    void  food_reproduction_function();
    bool  reproduce_food(Food* food);
    
    float calculate_spawn_chance() const;
    static bool can_food_reproduce(const Food* food) { return food->time_since_last_reproduced >= repro_cooldown && food->age >= reproductive_threshold;}

    bool  food_container_full() { return food_vector.size() >= max_food; }

    void handle_food_death();
};
