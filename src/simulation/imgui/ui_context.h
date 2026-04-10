#pragma once
#include "../../world/world.h"
#include "population_history.h"
#include <imgui.h>
#include <implot.h>

// All simulation state the UI tabs need, assembled once per frame.
// Tabs read/write through this — no direct Simulation dependency.
struct UIContext
{
    World& world;
    PopulationHistory& history;

    float    total_time_elapsed;
    unsigned ticks;
    float    fps;

    bool& rendering;
    bool& hide_panels;
    bool& open_extinction_window;
    bool& tick_frame_time;
    bool& running;
    bool& tracking;

    ImPlotColormap plot_colormap;
};