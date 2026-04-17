#pragma once
#include "../world/world_state.h"

#include <mutex>
#include <queue>

enum class CommandType
{
    // ── Toggle state (whole WorldToggles struct carried in payload) ───────
    SetToggles,

    // ── One-shot actions ──────────────────────────────────────────────────
    ResetSimulation,        // open extinction popup confirmed
    ClearAllFood,
    ClearAllProtozoa,

    // ── Spawn ─────────────────────────────────────────────────────────────
    SpawnRandom,            // spawn N random organisms
    SpawnFromFile,          // stub

    // ── Grid ──────────────────────────────────────────────────────────────
    SetGridResolution,      // apply new cell grid resolution

    // ── World values (not in WorldToggles) ───────────────────────────────
    SetSimulationSpeed,     // the speed multiplier slider
    SetWorldRadius,         // world resize stub
};

struct SpawnParams
{
    int   count = 10;
    bool  mutate = false;
    float mut_rate = 0.3f;
    float mut_range = 0.3f;
};


struct SimCommand
{
    CommandType  type;

    // Only one of these is meaningful per command — 
    // think of it like a union but without the hassle
    WorldToggles toggles{};     // for SetToggles
    SpawnParams  spawn{};       // for SpawnRandom
    float        float_val = 0; // for SetSimulationSpeed, SetWorldRadius
    int          int_val = 0; // for SetGridResolution
};

struct ImGuiContext
{
    WorldToggles& toggles;   // write toggles here freely
    std::mutex& cmd_mutex;
    std::queue<SimCommand>& commands;

    // Helper so tabs don't need to write the lock_guard boilerplate
    void push(SimCommand cmd)
    {
        std::lock_guard<std::mutex> lock(cmd_mutex);
        commands.push(std::move(cmd));
    }
};