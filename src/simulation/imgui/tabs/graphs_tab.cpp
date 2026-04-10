#include "graphs_tab.h"
#include "../population_history.h"
#include <imgui.h>
#include <implot.h>
#include <numeric>
#include <algorithm>

void GraphsTab::draw(UIContext& ctx)
{
    ImPlot::CreateContext();
    draw_shared_toolbar(ctx);

    if (!ImGui::BeginTabBar("##graph_tabs")) return;

    if (ImGui::BeginTabItem("Population")) { draw_population_tab(ctx);  ImGui::EndTabItem(); }
    if (ImGui::BeginTabItem("Generations")) { draw_generations_tab(ctx); ImGui::EndTabItem(); }
    if (ImGui::BeginTabItem("Misc")) { draw_misc_tab(ctx);        ImGui::EndTabItem(); }

    ImGui::EndTabBar();
}

void GraphsTab::draw_shared_toolbar(UIContext& ctx)
{
    const float live_x = ctx.total_time_elapsed;

    ImGui::SetNextItemWidth(160.f);
    ImGui::SliderFloat("Window (s)##g", &m_scroll_window_, 10.f, 600.f, "%.0fs");
    ImGui::SameLine(0, 16);

    if (m_recording_)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, { 0.7f, 0.2f, 0.2f, 1.f });
        if (ImGui::Button("Stop recording"))
        {
            m_recording_ = false;
            ctx.history.add_manual_event(live_x, "record end");
        }
        ImGui::PopStyleColor();
    }
    else if (ImGui::Button("Record from here"))
    {
        m_recording_ = true;
        m_record_start_ = live_x;
        ctx.history.add_manual_event(live_x, "record start", { 0.4f, 0.8f, 1.f, 1.f });
    }

    ImGui::SameLine(0, 16);
    if (ImGui::Button("Export CSV"))
        ctx.history.export_csv("population_export.csv");
}

void GraphsTab::draw_population_tab(UIContext& ctx)
{
    ImGui::Checkbox("Protozoa", &m_show_protozoa_); ImGui::SameLine(0, 12);
    ImGui::Checkbox("Food", &m_show_food_);     ImGui::SameLine(0, 12);
    ImGui::Checkbox("Total", &m_show_total_);    ImGui::SameLine(0, 12);
    ImGui::Checkbox("Bands", &m_show_bands_);

    const float live_x = ctx.total_time_elapsed;
    const float x_max = m_hover_paused_ ? m_paused_x_max_ : live_x;
    const float x_min = x_max - m_scroll_window_;

    constexpr ImPlotFlags     pf = ImPlotFlags_NoMenus | ImPlotFlags_NoBoxSelect | ImPlotFlags_NoMouseText;
    constexpr ImPlotAxisFlags yf = ImPlotAxisFlags_AutoFit;

    if (!ImPlot::BeginPlot("##pop", { -1.f, -1.f }, pf)) return;

    ImPlot::SetupAxes("Time (s)", "Count", ImPlotAxisFlags_None, yf);
    ImPlot::SetupAxisLimits(ImAxis_X1, x_min, x_max, ImGuiCond_Always);

    const int n = static_cast<int>(ctx.history.size());
    if (n > 0)
    {
        const float* t = ctx.history.time.data();

        if (m_show_bands_)
        {
            static std::vector<float> plo, phi, flo, fhi;
            if (m_show_protozoa_)
            {
                PopulationHistory::compute_band(ctx.history.protozoa, plo, phi);
                ImPlot::SetNextFillStyle({ 0.3f, 0.6f, 1.f, 1.f }, 0.15f);
                ImPlot::PlotShaded("##pb", t, plo.data(), phi.data(), n);
            }
            if (m_show_food_)
            {
                PopulationHistory::compute_band(ctx.history.food, flo, fhi);
                ImPlot::SetNextFillStyle({ 0.3f, 1.f, 0.4f, 1.f }, 0.12f);
                ImPlot::PlotShaded("##fb", t, flo.data(), fhi.data(), n);
            }
        }

        if (m_show_protozoa_)
        {
            ImPlot::SetNextLineStyle({ 0.3f, 0.6f, 1.f, 1.f });
            ImPlot::PlotLine("Protozoa", t, ctx.history.protozoa.data(), n);
        }
        if (m_show_food_)
        {
            ImPlot::SetNextLineStyle({ 0.3f, 1.f, 0.4f, 1.f });
            ImPlot::PlotLine("Food", t, ctx.history.food.data(), n);
        }
        if (m_show_total_)
        {
            ImPlot::SetNextLineStyle({ 0.65f, 0.65f, 0.65f, 1.f });
            ImPlot::PlotLine("Total", t, ctx.history.total.data(), n);
        }

        // extinction threshold
        {
            ImPlot::SetNextLineStyle({ 1.f, 0.25f, 0.25f, 0.8f }, 1.f);
            const float tx[2] = { t[0], t[n - 1] };
            const float ty[2] = { 10.f, 10.f };
            ImPlot::PlotLine("##ext", tx, ty, 2);
        }

        const float y_top = static_cast<float>(
            ctx.world.get_protozoa_count() + ctx.world.get_food_count());
        draw_event_markers(ctx, x_min, x_max, y_top);
        if (m_recording_ && m_record_start_ >= x_min)
            draw_record_region(x_max, y_top);
    }

    if (ImPlot::IsPlotHovered() && !m_hover_paused_)
    {
        m_hover_paused_ = true; m_paused_x_max_ = live_x;
    }
    else if (!ImPlot::IsPlotHovered())
        m_hover_paused_ = false;

    ImPlot::EndPlot();
}

void GraphsTab::draw_event_markers(UIContext& ctx, float x_min, float x_max, float y_top)
{
    for (const auto& ev : ctx.history.events)
    {
        if (ev.time < x_min || ev.time > x_max) continue;
        ImPlot::SetNextLineStyle(ev.color);
        const float ex[2] = { ev.time, ev.time };
        const float ey[2] = { 0.f, y_top };
        ImPlot::PlotLine("##ev", ex, ey, 2);

        if (ImPlot::IsPlotHovered())
        {
            const ImPlotPoint mp = ImPlot::GetPlotMousePos();
            if (std::abs(static_cast<float>(mp.x) - ev.time) < 0.8f)
                ImGui::SetTooltip("%s @ %.1fs", ev.label.c_str(), ev.time);
        }
    }
}

void GraphsTab::draw_record_region(float x_max, float y_top)
{
    ImPlot::SetNextFillStyle({ 0.4f, 0.8f, 1.f, 1.f }, 0.07f);
    const float rx[2] = { m_record_start_, x_max };
    const float rlo[2] = { 0.f, 0.f };
    const float rhi[2] = { y_top, y_top };
    ImPlot::PlotShaded("##rec", rx, rlo, rhi, 2);
}

void GraphsTab::draw_generations_tab(UIContext& ctx)
{
    const auto& gen_data = ctx.world.get_generation_distribution();
    constexpr ImPlotFlags hf = ImPlotFlags_NoMenus | ImPlotFlags_NoBoxSelect | ImPlotFlags_NoMouseText;
    const float plot_h = ImGui::GetContentRegionAvail().y - ImGui::GetFrameHeightWithSpacing() * 2.f;

    if (!gen_data.empty() && ImPlot::BeginPlot("##gen_hist", { -1.f, plot_h }, hf))
    {
        ImPlot::SetupAxes("Generation", "Count", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
        ImPlot::SetNextFillStyle({ 0.4f, 0.7f, 1.f, 1.f }, 0.75f);
        ImPlot::PlotHistogram("Generation dist.", gen_data.data(),
            static_cast<int>(gen_data.size()),
            ImPlotBin_Sturges, 1.0, ImPlotRange{});
        ImPlot::EndPlot();
    }
    else if (gen_data.empty())
        ImGui::TextDisabled("No organisms alive.");

    if (!gen_data.empty())
    {
        const float sum = std::accumulate(gen_data.begin(), gen_data.end(), 0.f);
        const float mean = sum / static_cast<float>(gen_data.size());
        const float mx = *std::max_element(gen_data.begin(), gen_data.end());
        ImGui::Separator();
        ImGui::BeginChild("##gstats", { -1.f, ImGui::GetFrameHeightWithSpacing() },
            false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        ImGui::Text("Mean: %.2f", mean);
        ImGui::SameLine(0, 20); ImGui::Text("Max: %.0f", mx);
        ImGui::SameLine(0, 20); ImGui::Text("N: %d", (int)gen_data.size());
        ImGui::EndChild();
    }
}

void GraphsTab::draw_misc_tab(UIContext& ctx)
{
    ImGui::Checkbox("Mut Rate", &m_show_mut_rate_);    ImGui::SameLine(0, 10);
    ImGui::Checkbox("Mut Range", &m_show_mut_range_);   ImGui::SameLine(0, 10);
    ImGui::Checkbox("Offspring", &m_show_offspring_);
    ImGui::Checkbox("Lifetime", &m_show_lifetime_);    ImGui::SameLine(0, 10);
    ImGui::Checkbox("Avg Cells", &m_show_avg_cells_);   ImGui::SameLine(0, 10);
    ImGui::Checkbox("Avg Springs", &m_show_avg_springs_); ImGui::SameLine(0, 10);
    ImGui::Checkbox("Avg Energy", &m_show_avg_energy_);

    const MiscSeries& ms = ctx.history.misc;
    const int n = static_cast<int>(ms.mut_rate.size());
    if (n == 0 || static_cast<int>(ctx.history.time.size()) != n)
    {
        ImGui::TextDisabled("No misc data — enable Track Stats and wait.");
        return;
    }

    constexpr ImPlotFlags pf = ImPlotFlags_NoMenus | ImPlotFlags_NoBoxSelect | ImPlotFlags_NoMouseText;
    if (!ImPlot::BeginPlot("##misc", { -1.f, -1.f }, pf)) return;

    ImPlot::SetupAxes("Time (s)", "Rate [0, 1]", ImPlotAxisFlags_None, ImPlotAxisFlags_AutoFit);
    ImPlot::SetupAxis(ImAxis_Y2, "Count / Value", ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_Opposite);

    const float* t = ctx.history.time.data();

    auto plot_y1 = [&](const char* name, const float* data, ImVec4 col)
        {
            ImPlot::SetAxes(ImAxis_X1, ImAxis_Y1);
            ImPlot::SetNextLineStyle(col);
            ImPlot::PlotLine(name, t, data, n);
        };
    auto plot_y2 = [&](const char* name, const float* data, ImVec4 col)
        {
            ImPlot::SetAxes(ImAxis_X1, ImAxis_Y2);
            ImPlot::SetNextLineStyle(col);
            ImPlot::PlotLine(name, t, data, n);
        };

    if (m_show_mut_rate_)    plot_y1("Mut Rate", ms.mut_rate.data(), { 1.f,  0.5f, 0.2f, 1.f });
    if (m_show_mut_range_)   plot_y1("Mut Range", ms.mut_range.data(), { 1.f,  0.8f, 0.2f, 1.f });
    if (m_show_offspring_)   plot_y2("Offspring", ms.avg_offspring.data(), { 0.4f, 1.f,  0.6f, 1.f });
    if (m_show_lifetime_)    plot_y2("Lifetime", ms.avg_lifetime.data(), { 0.6f, 0.4f, 1.f,  1.f });
    if (m_show_avg_cells_)   plot_y2("Avg Cells", ms.avg_cells.data(), { 0.3f, 0.8f, 1.f,  1.f });
    if (m_show_avg_springs_) plot_y2("Avg Springs", ms.avg_springs.data(), { 0.9f, 0.3f, 0.9f, 1.f });
    if (m_show_avg_energy_)  plot_y2("Avg Energy", ms.avg_energy.data(), { 1.f,  0.9f, 0.3f, 1.f });

    ImPlot::EndPlot();
}