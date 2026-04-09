#include "../simulation.h"



void Simulation::imgui_population_graph()
{
    // ── Panel settings (tune here, not buried in logic) ───────────────────
    struct Settings
    {
        ImVec2 panel_pos = { 10.f,  800.f };
        ImVec2 panel_size = { 380.f, 260.f };
        float  scroll_window = 60.f;    // runtime-adjustable below
        int    extinction_thresh = 10;
        bool   show_protozoa = true;
        bool   show_food = true;
        bool   show_total = true;
        bool   show_bands = true;
    };
    static Settings s{};

    // ── Extinction risk state ─────────────────────────────────────────────
    const int   protozoa_count = m_world_.get_protozoa_count();
    const int   food_count = m_world_.get_food_count();
    const bool  at_risk = protozoa_count <= s.extinction_thresh;

    // ── X-axis ────────────────────────────────────────────────────────────
    static bool  hover_paused = false;
    static float paused_x_max = 0.f;

    const float live_x_max = m_total_time_elapsed_;
    const float x_max = hover_paused ? paused_x_max : live_x_max;
    const float x_min = x_max - s.scroll_window;

    // ── Record-from-here ──────────────────────────────────────────────────
    static bool  recording = false;
    static float record_start = 0.f;

    // ── Window ────────────────────────────────────────────────────────────
    ImGui::SetNextWindowPos(s.panel_pos, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(s.panel_size, ImGuiCond_FirstUseEver);
    ImGui::Begin("Population", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoNav);

    // ── Stats bar ─────────────────────────────────────────────────────────
    auto stat = [](const char* label, auto value, bool warn = false) {
        if (warn) ImGui::PushStyleColor(ImGuiCol_Text, { 1.f, 0.3f, 0.3f, 1.f });
        ImGui::Text(label, value);
        if (warn) ImGui::PopStyleColor();
        ImGui::SameLine(0, 16);
        };

    stat("Protozoa: %d", protozoa_count, at_risk);
    stat("Food: %d", food_count);
    stat("Gen: %.2f", m_world_.average_generation_);
    stat("Frame: %d", m_ticks_);
    ImGui::NewLine();

    // ── Scroll window slider + record toggle ──────────────────────────────
    ImGui::SetNextItemWidth(160.f);
    ImGui::SliderFloat("Window (s)", &s.scroll_window, 10.f, 600.f, "%.0f s");
    ImGui::SameLine(0, 16);

    if (recording)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, { 0.7f, 0.2f, 0.2f, 1.f });
        if (ImGui::Button("Stop recording"))
        {
            recording = false;
            m_history_.add_manual_event(live_x_max, "record end");
        }
        ImGui::PopStyleColor();
    }
    else
    {
        if (ImGui::Button("Record from here"))
        {
            recording = true;
            record_start = live_x_max;
            m_history_.add_manual_event(live_x_max, "record start",
                { 0.4f, 0.8f, 1.f, 1.f });
        }
    }

    ImGui::SameLine(0, 16);
    if (ImGui::Button("Export CSV"))
        m_history_.export_csv("population_export.csv");

    // ── Tabs ──────────────────────────────────────────────────────────────
    if (!ImGui::BeginTabBar("##pop_tabs"))
    {
        ImGui::End();
        return;
    }

    // ════════════════════════════════════════════════════════════════════════
    // Tab 1 — Line graph
    // ════════════════════════════════════════════════════════════════════════
    if (ImGui::BeginTabItem("Population"))
    {
        // Series toggles
        ImGui::Checkbox("Protozoa", &s.show_protozoa);
        ImGui::SameLine(0, 12);
        ImGui::Checkbox("Food", &s.show_food);
        ImGui::SameLine(0, 12);
        ImGui::Checkbox("Total", &s.show_total);
        ImGui::SameLine(0, 12);
        ImGui::Checkbox("Bands", &s.show_bands);

        constexpr ImPlotFlags     plot_flags = ImPlotFlags_NoMenus
            | ImPlotFlags_NoBoxSelect
            | ImPlotFlags_NoMouseText;
        constexpr ImPlotAxisFlags x_flags = ImPlotAxisFlags_None;
        constexpr ImPlotAxisFlags y_flags = ImPlotAxisFlags_AutoFit;

        if (ImPlot::BeginPlot("##population", { -1.f, -1.f }, plot_flags))
        {
            ImPlot::SetupAxes("Time (s)", "Count", x_flags, y_flags);
            ImPlot::SetupAxisLimits(ImAxis_X1, x_min, x_max, ImGuiCond_Always);

            const int n = static_cast<int>(m_history_.size());
            if (n > 0)
            {
                const float* t = m_history_.time.data();

                

                // ── Lines ─────────────────────────────────────────────────
                if (s.show_protozoa)
                {
                    ImPlot::SetNextLineStyle({ 0.3f, 0.6f, 1.f, 1.f });
                    ImPlot::PlotLine("Protozoa", t, m_history_.protozoa.data(), n);
                }
                if (s.show_food)
                {
                    ImPlot::SetNextLineStyle({ 0.3f, 1.f, 0.4f, 1.f });
                    ImPlot::PlotLine("Food", t, m_history_.food.data(), n);
                };

                if (s.show_total)
                {
                    ImPlot::PushStyleColor(ImPlotCol_Line, { 0.65f, 0.65f, 0.65f, 1.f });
                    ImPlot::PlotLine("Total", t, m_history_.total.data(), n);
                    ImPlot::PopStyleColor();
                }

                // ── Extinction risk threshold ──────────────────────────────
                if (s.show_protozoa)
                {
                    const double thresh = static_cast<double>(s.extinction_thresh);
                    ImPlot::PushStyleColor(ImPlotCol_Line, { 1.f, 0.25f, 0.25f, 0.8f });
                    ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 1.f);
                    // Horizontal threshold line via two-point PlotLine
                    const float thresh_x[2] = { t[0],  t[n - 1] };
                    const float thresh_y[2] = { static_cast<float>(thresh),
                                                static_cast<float>(thresh) };
                    ImPlot::PlotLine("##extinction", thresh_x, thresh_y, 2);
                    ImPlot::PopStyleVar();
                    ImPlot::PopStyleColor();

                    if (at_risk)
                    {
                        ImPlot::PushStyleColor(ImPlotCol_InlayText,
                            { 1.f, 0.3f, 0.3f, 1.f });
                        ImPlot::PlotText("! extinction risk",
                            x_min + s.scroll_window * 0.5f,
                            static_cast<float>(thresh) + 2.f);
                        ImPlot::PopStyleColor();
                    }
                }

                // ── Event markers ─────────────────────────────────────────
                for (const auto& ev : m_history_.events)
                {
                    if (ev.time < x_min || ev.time > x_max) continue;

                    ImPlot::PushStyleColor(ImPlotCol_Line, ev.color);
                    const float ex[2] = { ev.time, ev.time };
                    const float ey[2] = { 0.f, static_cast<float>(
                        m_world_.get_protozoa_count() +
                        m_world_.get_food_count()) };
                    ImPlot::PlotLine("##ev", ex, ey, 2);
                    ImPlot::PopStyleColor();

                    if (ImPlot::IsPlotHovered())
                    {
                        const ImPlotPoint mp = ImPlot::GetPlotMousePos();
                        if (std::abs(static_cast<float>(mp.x) - ev.time) < 0.8f)
                            ImGui::SetTooltip("%s @ %.1fs", ev.label.c_str(), ev.time);
                    }
                }

                // ── Record region highlight ────────────────────────────────
                if (recording && record_start >= x_min)
                {
                    ImPlot::PushStyleColor(ImPlotCol_Fill, { 0.4f, 0.8f, 1.f, 0.07f });
                    const float rx[2] = { record_start, x_max };
                    const float rlo[2] = { 0.f, 0.f };
                    const float rhi[2] = { static_cast<float>(
                        m_world_.get_protozoa_count() +
                        m_world_.get_food_count()), static_cast<float>(
                        m_world_.get_protozoa_count() +
                        m_world_.get_food_count()) };
                    ImPlot::PlotShaded("##rec", rx, rlo, rhi, 2);
                    ImPlot::PopStyleColor();
                }
            }

            // ── Pause on hover ─────────────────────────────────────────────
            if (ImPlot::IsPlotHovered() && !hover_paused)
            {
                hover_paused = true;
                paused_x_max = live_x_max;
            }
            else if (!ImPlot::IsPlotHovered())
            {
                hover_paused = false;
            }

            ImPlot::EndPlot();
        }

        ImGui::EndTabItem();
    }

    // ════════════════════════════════════════════════════════════════════════
    // Tab 2 — Generation histogram
    // ════════════════════════════════════════════════════════════════════════
    if (ImGui::BeginTabItem("Generations"))
    {
        // m_world_ must expose a way to iterate per-organism generation values.
        // Assumed interface: get_generation_distribution() -> const std::vector<float>&
        const auto& gen_data = m_world_.get_generation_distribution();

        constexpr ImPlotFlags hist_flags = ImPlotFlags_NoMenus
            | ImPlotFlags_NoBoxSelect
            | ImPlotFlags_NoMouseText;

        if (!gen_data.empty() &&
            ImPlot::BeginPlot("##gen_hist", { -1.f, -1.f }, hist_flags))
        {
            ImPlot::SetupAxes("Generation", "Count",
                ImPlotAxisFlags_AutoFit,
                ImPlotAxisFlags_AutoFit);

            ImPlot::PushStyleColor(ImPlotCol_Fill, { 0.4f, 0.7f, 1.f, 0.75f });
            ImPlot::PlotHistogram("Generation dist.",
                gen_data.data(),
                static_cast<int>(gen_data.size()),
                ImPlotBin_Sturges,   // auto bin count
                1.0,                 // bar scale
                ImPlotRange{});      // auto range
            ImPlot::PopStyleColor();
            ImPlot::EndPlot();
        }
        else if (gen_data.empty())
        {
            ImGui::TextDisabled("No organisms alive.");
        }

        // Numeric summary alongside the histogram
        if (!gen_data.empty())
        {
            const float sum = std::accumulate(gen_data.begin(), gen_data.end(), 0.f);
            const float mean = sum / static_cast<float>(gen_data.size());
            const float mx = *std::max_element(gen_data.begin(), gen_data.end());
            ImGui::Separator();
            ImGui::BeginChild("##gen_stats", { -1.f, ImGui::GetFrameHeightWithSpacing() },
                false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
            ImGui::Text("Mean: %.2f", mean);
            ImGui::SameLine(0, 20);
            ImGui::Text("Max: %.0f", mx);
            ImGui::SameLine(0, 20);
            ImGui::Text("N: %d", static_cast<int>(gen_data.size()));
            ImGui::EndChild();
        }

        ImGui::EndTabItem();
    }

    ImGui::EndTabBar();
    ImGui::End();
}