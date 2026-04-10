#include "simulation_tab.h"
#include "../helpers/plot_utils.h"
#include "../helpers/confirm_button.h"
#include <imgui.h>

void SimulationTab::draw(UIContext& ctx)
{
    draw_playback(ctx);
    ImGui::Spacing();
    draw_fast_forward();
    ImGui::Spacing();
    draw_world_settings(ctx);
    ImGui::Spacing();
    draw_io_stubs();
    ImGui::Spacing();
    ImGui::SeparatorText("Keybinds");
    draw_keybind_table();
}

void SimulationTab::draw_playback(UIContext& ctx)
{
    ImGui::SeparatorText("Playback");
    ImGui::Text("Time: %s  |  Frame: %u",
        PlotUtils::format_time(ctx.total_time_elapsed).c_str(), ctx.ticks);

    if (ImGui::Button(ctx.world.paused ? "Resume [Space]" : "Pause  [Space]", { 160.f, 0.f }))
        ctx.world.paused = !ctx.world.paused;
    ImGui::SameLine();
    if (ImGui::Button("Step [O]"))
    {
        ctx.tick_frame_time = true;
        ctx.world.paused = true;
    }

    ImGui::SetNextItemWidth(200.f);
    ImGui::SliderFloat("Speed##sim", &m_speed_, 0.1f, 10.f, "%.1fx");
    ImGui::TextDisabled("(wire m_speed_ to tick rate multiplier)");

    ImGui::Spacing();
    if (ImGui::Button("Reset Simulation##sim", { -1.f, 0.f }))
        ctx.open_extinction_window = true;
}

void SimulationTab::draw_fast_forward()
{
    ImGui::SeparatorText("Fast Forward");

    if (m_fast_fwd_) ImGui::PushStyleColor(ImGuiCol_Button, { 0.6f, 0.2f, 0.2f, 1.f });
    if (ImGui::Button(m_fast_fwd_ ? "Stop Fast Forward##ff" : "Start Fast Forward##ff", { -1.f, 0.f }))
        m_fast_fwd_ = !m_fast_fwd_;
    if (m_fast_fwd_) ImGui::PopStyleColor();

    const char* conds[] = { "Time elapsed (s)", "Population target", "Generation target" };
    int c = static_cast<int>(m_ff_cond_);
    ImGui::SetNextItemWidth(180.f);
    if (ImGui::Combo("Condition##ff", &c, conds, 3))
        m_ff_cond_ = static_cast<FFCondition>(c);
    ImGui::SetNextItemWidth(120.f);
    ImGui::InputFloat("Target##ff", &m_ff_target_, 10.f, 100.f, "%.0f");
    ImGui::TextDisabled("TODO: disable rendering + max tick rate when active");
}

void SimulationTab::draw_world_settings(UIContext& ctx)
{
    ImGui::SeparatorText("World");

    ImGui::SetNextItemWidth(-1.f);
    ImGui::SliderFloat("Min Speed##sim", &ctx.world.min_speed, 0.f, 135.f, "%.1f");

    float ds = ctx.world.delta_min_speed * 1000.f;
    ImGui::SetNextItemWidth(-1.f);
    if (ImGui::SliderFloat("Delta Speed (x1000)##sim", &ds, 0.2f, 2.f))
        ctx.world.delta_min_speed = ds / 1000.f;

    static float world_radius = WorldSettings::bounds_radius;
    ImGui::SetNextItemWidth(-1.f);
    ImGui::InputFloat("World Radius##sim", &world_radius, 1000.f, 10000.f, "%.0f");
    ImGui::TextDisabled("TODO: wire to World::resize(radius)");
}

void SimulationTab::draw_io_stubs()
{
    ImGui::SeparatorText("Save / Load");
    if (ImGui::Button("Save World##sim", { -1.f, 0.f })) { /* TODO */ }
    if (ImGui::Button("Load World##sim", { -1.f, 0.f })) { /* TODO */ }
    if (ImGui::Button("Save Settings to JSON##sim", { -1.f, 0.f })) { /* TODO */ }
    if (ImGui::Button("Load Settings from JSON##sim", { -1.f, 0.f })) { /* TODO */ }
}

void SimulationTab::draw_keybind_table()
{
    if (!ImGui::BeginTable("##keys", 2,
        ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchProp))
        return;

    ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthFixed, 100.f);
    ImGui::TableHeadersRow();

    auto row = [](const char* a, const char* k)
        {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("%s", a);
            ImGui::TableSetColumnIndex(1); ImGui::TextDisabled("%s", k);
        };

    row("Pause / Resume", "Space");
    row("Step one frame", "O");
    row("Toggle rendering", "R");
    row("Hide panels", "Q");
    row("Exit", "Esc");
    row("Toggle cell grid", "G");
    row("Toggle food grid", "F");
    row("Toggle collisions", "C");
    row("Simple mode", "S");
    row("Debug mode", "D");
    row("Track stats", "T");
    row("Skeleton mode", "K (debug)");
    row("Bounding boxes", "B (debug)");
    row("Zoom", "Scroll");
    row("Pan", "LMB drag");
    row("Select organism", "LMB click");

    ImGui::EndTable();
}