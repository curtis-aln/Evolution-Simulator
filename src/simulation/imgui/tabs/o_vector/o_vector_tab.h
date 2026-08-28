#pragma once
#include "../i_tab.h"
#include "../../../../Utils/o_vec/o_vec_debug.h"

// ─────────────────────────────────────────────────────────────────────────────
//  OVecDebugTab  —  ImGui control-panel tab that visualises all four
//  OVecDebug instances live:  cells, food, bodies (cells+food), and springs.
//
//  Sub-tabs
//  ┌────────────┬───────────────────────────────────────────────────────────┐
//  │ Overview   │ Side-by-side capacity + fill bars for all four vectors    │
//  │ Capacity   │ Detailed slot counts, memory usage, resize events         │
//  │ Churn      │ Per-slot reuse heatmap + top-N most recycled slots        │
//  │ Snapshots  │ Take / diff named snapshots; delta table                  │
//  │ Integrity  │ Run integrity checks on-demand, show pass/fail log        │
//  └────────────┴───────────────────────────────────────────────────────────┘
//o_vec
//  Usage:
//      OVecDebugTab tab(dbg_cells, dbg_food, dbg_bodies, dbg_springs);
//      // In your draw loop:
//      tab.draw(snap, ctx);
// ─────────────────────────────────────────────────────────────────────────────


// ─────────────────────────────────────────────────────────────────────────────
//  Design constants
// ─────────────────────────────────────────────────────────────────────────────

static constexpr float k_bar_height = 14.f;   // main capacity bars
static constexpr float k_heatmap_min_w = 6.f;     // minimum slot cell width in heatmap
static constexpr int   k_history_scroll = 128;     // max history samples kept for sparkline

// Colours reused across the whole tab
static constexpr ImVec4 k_col_cells = { 0.35f, 0.75f, 0.35f, 1.f }; // green
static constexpr ImVec4 k_col_food = { 0.85f, 0.65f, 0.15f, 1.f }; // amber
static constexpr ImVec4 k_col_bodies = { 0.45f, 0.65f, 0.95f, 1.f }; // blue
static constexpr ImVec4 k_col_springs = { 0.80f, 0.40f, 0.75f, 1.f }; // purple
static constexpr ImVec4 k_col_warn = { 1.00f, 0.50f, 0.10f, 1.f }; // orange warning
static constexpr ImVec4 k_col_ok = { 0.40f, 0.90f, 0.40f, 1.f }; // integrity pass
static constexpr ImVec4 k_col_fail = { 0.95f, 0.25f, 0.25f, 1.f }; // integrity fail

// Vector display names and their matching colours — indexed 0-3 everywhere.
static const char* k_vec_names[4] = { "Cells", "Food", "Bodies", "Springs" };
static const ImVec4   k_vec_colours[4] = { k_col_cells, k_col_food, k_col_bodies, k_col_springs };


// ─────────────────────────────────────────────────────────────────────────────
//  Internal helper utilities  (file-local, no header needed)
// ─────────────────────────────────────────────────────────────────────────────

// A bar that goes green→yellow→red as fill_ratio increases: 0=green, 1=red.
static ImVec4 fill_heat_color(float ratio)
{
    ratio = std::clamp(ratio, 0.f, 1.f);
    return ratio > 0.5f
        ? ImVec4{ 1.f,       2.f * (1.f - ratio), 0.1f, 1.f }
    : ImVec4{ 2.f * ratio, 1.f,               0.1f, 1.f };
}

// Fragmentation goes blue (0 = compact) → red (1 = fragmented).
static ImVec4 frag_color(float score)
{
    score = std::clamp(score, 0.f, 1.f);
    return { score, 0.2f, 1.f - score, 1.f };
}

// Formats a byte count as "X B / X KB / X MB".
static void format_bytes(char* buf, int buf_sz, size_t bytes)
{
    if (bytes < 1024)             snprintf(buf, buf_sz, "%zu B", bytes);
    else if (bytes < 1024 * 1024)      snprintf(buf, buf_sz, "%.1f KB", bytes / 1024.f);
    else                               snprintf(buf, buf_sz, "%.2f MB", bytes / (1024.f * 1024.f));
}

// A thin separator line with optional label.
static void section(const char* title)
{
    ImGui::Spacing();
    ImGui::TextDisabled("%s", title);
    ImGui::Separator();
    ImGui::Spacing();
}

// Forward-declare the four object types so we can hold typed debug refs.
struct Cell;
struct Food;
struct Body;
struct Spring;

class OVecDebugTab : public ITab
{
public:
    // ── Construction ─────────────────────────────────────────────────────────

    OVecDebugTab()
    {
        m_integrity_log_.reserve(32);
    }

    // ── ITab interface ────────────────────────────────────────────────────────

    const char* label() const override { return "O_Vec Debug"; }
    void        draw(const SimSnapshot& snap, ImGuiContext& ctx) override
    {
        if (!ImGui::BeginTabBar("##ov_debug_tabs"))
            return;

        if (ImGui::BeginTabItem("Overview")) { draw_overview_tab(snap, ctx);   ImGui::EndTabItem(); }
        if (ImGui::BeginTabItem("Capacity")) { draw_capacity_tab(snap, ctx);   ImGui::EndTabItem(); }
        if (ImGui::BeginTabItem("Churn")) { draw_churn_tab(snap, ctx);      ImGui::EndTabItem(); }
        if (ImGui::BeginTabItem("Snapshots")) { draw_snapshots_tab(snap, ctx);  ImGui::EndTabItem(); }
        if (ImGui::BeginTabItem("Integrity")) { draw_integrity_tab(snap, ctx);  ImGui::EndTabItem(); }

        ImGui::EndTabBar();
    }

private:
    // ── Typed debug references (non-owning) ──────────────────────────────────

    OVecDebug<Cell> m_dbg_cells_;
    OVecDebug<Food> m_dbg_food_;
    OVecDebug<Body> m_dbg_bodies_;
    OVecDebug<Spring> m_dbg_springs_;

    // ── Per-tab state ─────────────────────────────────────────────────────────

    // Snapshot sub-tab: indices for the two snapshots being diffed
    int  m_snap_a_idx_ = 0;
    int  m_snap_b_idx_ = 0;

    // Snapshot sub-tab: which vector's snapshot list is shown (0-3)
    int  m_snap_vec_sel_ = 0;

    // Churn sub-tab: top-N slider
    int  m_churn_top_n_ = 20;

    // Churn sub-tab: which vector to inspect (0-3)
    int  m_churn_vec_sel_ = 0;

    // Integrity sub-tab: accumulated log lines
    std::vector<std::string> m_integrity_log_;
    bool m_integrity_all_pass_ = true;

    // ── Sub-tab draw functions ────────────────────────────────────────────────

    void draw_overview_tab(const SimSnapshot& snap, ImGuiContext& ctx);
    void draw_capacity_tab(const SimSnapshot& snap, ImGuiContext& ctx);
    void draw_churn_tab(const SimSnapshot& snap, ImGuiContext& ctx);
    void draw_snapshots_tab(const SimSnapshot& snap, ImGuiContext& ctx);
    void draw_integrity_tab(const SimSnapshot& snap, ImGuiContext& ctx);
};