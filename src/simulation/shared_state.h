#pragma once

#include "../Protozoa/cell.h"
#include "../Protozoa/spring.h"
#include "../world/world_state.h"
#include "../Protozoa/Protozoa.h"
#include "imgui/population_history.h"

// [Threaded] Update()
// [Threaded] ImGUI rendered statistics and render data are written into a SimSnapshot struct managed by SharedState
// [Main Thread] This data is then read and copied into another SimSnapshot inside the SharedInput class
// [Main Thread] This copied data is then used for rendering and passed through ImGui, modifying its data
// [Threaded] At the start of the next Update(), it checks if the main thread has published a new snapshot, and if so, it copies the data into the world.

struct SimSnapshot
{
    std::vector<sf::Vector2f> cell_positions;
    std::vector<sf::Color>    cell_colors;
    std::vector<float>        cell_radii;

    std::vector<sf::Vector2f> food_positions;
    std::vector<sf::Color>    food_colors;
    std::vector<float>        food_radii;

    // graph data
    float total_time_elapsed = 0.f;

    int iterations_ = 0;
    float    m_total_time_elapsed_ = 0.f;

    WorldToggles toggles;
    WorldStatistics stats;
    RenderData render;
    SpatialGridData food_grid;
    SpatialGridData cell_grid;
    PopulationHistory history;

    bool selected_a_protozoa = false;
    bool clone = false;
    Protozoa protozoa;
};

struct SharedState
{
    SimSnapshot       buffers[2];
    std::atomic<int>  latest_written{ 0 };  // index of the last fully written buffer

    // Called by the update thread to know which buffer to write into.
    // Always the buffer that the render thread is NOT currently reading.
    int get_write_buffer_index() const
    {
        return 1 - latest_written.load(std::memory_order_relaxed);
    }

    // Called by the render thread to know which buffer to read from.
    // Always the last fully written buffer.
    int get_read_buffer_index() const
    {
        return latest_written.load(std::memory_order_acquire);
    }

    // Called by the update thread after it finishes writing.
    // Flips latest_written so the render thread picks up the new data.
    void publish_write()
    {
        latest_written.store(get_write_buffer_index(), std::memory_order_release);
    }
};
