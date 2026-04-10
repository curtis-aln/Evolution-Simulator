#include "../simulation.h"
#include "../../Protozoa/genetics/CellGenome.h"
#include "../../Protozoa/genetics/SpringGenome.h"



// ─────────────────────────────────────────────────────────────────────────────
//  Fill 120 samples with A·sin(B·t + C) + D, clamped to [lo, hi]
// ─────────────────────────────────────────────────────────────────────────────
static void fill_sinwave(float* out,
    float A, float B, float C, float D,
    float lo, float hi)
{
    for (int i = 0; i < 120; ++i)
        out[i] = std::clamp(A * sinf(B * (float)i + C) + D, lo, hi);
}


// ─────────────────────────────────────────────────────────────────────────────
//  PlotLines with a vertical red line + filled circle dot at the playhead.
//  The graph is always drawn from sample 0 so the wave is stable;
//  the marker sweeps across it as frames_alive advances.
// ─────────────────────────────────────────────────────────────────────────────
static void sinwave_graph(const char* id, float* buf, int playhead, float y_min, float y_max, ImVec2 size = { -1.f, 52.f })
{
    ImGui::PlotLines(id, buf, 120, 0, nullptr, y_min, y_max, size);

    const ImVec2 rmin = ImGui::GetItemRectMin();
    const ImVec2 rmax = ImGui::GetItemRectMax();

    // x: lerp across the rect proportional to playhead / 119
    const float xf = rmin.x + (playhead / 119.f) * (rmax.x - rmin.x);

    // y: map sample value into rect (screen y is flipped)
    const float t = 1.f - std::clamp((buf[playhead] - y_min) / (y_max - y_min), 0.f, 1.f);
    const float yf = rmin.y + t * (rmax.y - rmin.y);

    auto* dl = ImGui::GetWindowDrawList();
    dl->AddLine({ xf, rmin.y }, { xf, rmax.y }, IM_COL32(255, 65, 65, 210), 1.5f);
    dl->AddCircleFilled({ xf, yf }, 3.5f, IM_COL32(255, 80, 80, 255));
}


// ─────────────────────────────────────────────────────────────────────────────
void Simulation::imgui_debug_panel(Cell* /*selected_cell*/, Protozoa* p)
// ─────────────────────────────────────────────────────────────────────────────
{
    if (!p) return;

    // Reset list selections when we switch to a different protozoa
    static int last_id = -1;
    static int sel_cell_idx = 0;
    static int sel_spring_idx = 0;

    if (p->id != last_id)
    {
        last_id = p->id;
        sel_cell_idx = 0;
        sel_spring_idx = 0;
    }

    // Wide landscape window — default sits along the bottom of the monitor
    ImGui::SetNextWindowSize({ 1200.f, 272.f }, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos({ 10.f, 760.f }, ImGuiCond_FirstUseEver);

    // Tighten item spacing for the whole window
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 6.f, 2.f });

    ImGui::Begin("Protozoa Inspector");

    if (!ImGui::BeginTabBar("##proto_tabs"))
    {
        ImGui::End();
        ImGui::PopStyleVar();
        return;
    }


    // ══════════════════════════════════════════════════════════════════════════
    //  TAB 1 — OVERVIEW   (4 equal columns)
    // ══════════════════════════════════════════════════════════════════════════
    if (ImGui::BeginTabItem("Overview"))
    {
        const float cw = (ImGui::GetContentRegionAvail().x - 18.f) / 4.f;
        const float ch = -1.f;

        // ── Identity ──────────────────────────────────────────────────────────
        ImGui::BeginChild("OV_id", { cw, ch }, true);
        ImGui::TextDisabled("Identity");
        ImGui::Text("ID       %d", p->id);
        ImGui::Text("Age      %u fr", p->frames_alive);
        ImGui::Text("Cells    %d", (int)p->get_cells().size());
        ImGui::Text("Springs  %d", (int)p->get_springs().size());
        ImGui::Text("Birth    (%.0f, %.0f)", p->birth_location.x, p->birth_location.y);
        ImGui::EndChild();   ImGui::SameLine();

        // ── Energy ────────────────────────────────────────────────────────────
        ImGui::BeginChild("OV_en", { cw, ch }, true);
        ImGui::TextDisabled("Energy");
        const float ef = std::clamp(p->get_energy() / initial_energy, 0.f, 1.f);
        ImVec4 bc = ef > 0.5f
            ? ImVec4{ 2.f * (1.f - ef), 1.f,     0.2f, 1.f }
        : ImVec4{ 1.f,          2.f * ef,   0.2f, 1.f };
        char elbl[32];
        snprintf(elbl, sizeof(elbl), "%.1f / %.0f", p->get_energy(), initial_energy);
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, bc);
        ImGui::ProgressBar(ef, { -1.f, 10.f }, elbl);
        ImGui::PopStyleColor();
        ImGui::Text("Springs cost  %.4f", p->energy_lost_to_springs);
        ImGui::EndChild();   ImGui::SameLine();

        // ── Locomotion ────────────────────────────────────────────────────────
        ImGui::BeginChild("OV_loc", { cw, ch }, true);
        ImGui::TextDisabled("Locomotion");
        ImGui::Text("Speed    %.4f", p->velocity.length());
        ImGui::Text("Vel X    %.3f", p->velocity.x);
        ImGui::Text("Vel Y    %.3f", p->velocity.y);
        ImGui::EndChild();   ImGui::SameLine();

        // ── Reproduction & Feeding ────────────────────────────────────────────
        ImGui::BeginChild("OV_rep", { -1.f, ch }, true);
        ImGui::TextDisabled("Reproduction & Feeding");
        ImGui::Text("Offspring    %d", p->offspring_count);
        ImGui::Text("Food eaten   %u", p->total_food_eaten);
        ImGui::Text("Since repr   %zu fr", p->time_since_last_reproduced);
        ImGui::EndChild();

        ImGui::EndTabItem();
    }


    // ══════════════════════════════════════════════════════════════════════════
    //  TAB 2 — CELLS   (list | stats+colors | wave+sliders)
    // ══════════════════════════════════════════════════════════════════════════
    if (ImGui::BeginTabItem("Cells"))
    {
        auto& cells = p->get_cells();

        if (cells.empty())
        {
            ImGui::TextDisabled("No cells.");
        }
        else
        {
            sel_cell_idx = std::min(sel_cell_idx, (int)cells.size() - 1);
            Cell& c = cells[sel_cell_idx];
            const float ph = -1.f;

            // ── Cell list ─────────────────────────────────────────────────────
            ImGui::BeginChild("CL_list", { 78.f, ph }, true);
            for (int i = 0; i < (int)cells.size(); ++i)
            {
                const Cell& ci = cells[i];
                ImVec4 dot = { ci.cell_outer_color.r / 255.f,
                               ci.cell_outer_color.g / 255.f,
                               ci.cell_outer_color.b / 255.f, 1.f };
                ImGui::PushStyleColor(ImGuiCol_Text, dot);
                ImGui::Text("●");
                ImGui::PopStyleColor();
                ImGui::SameLine();
                char lbl[12]; snprintf(lbl, sizeof(lbl), "C%d", i);
                if (ImGui::Selectable(lbl, sel_cell_idx == i))
                    sel_cell_idx = i;
            }
            ImGui::EndChild();   ImGui::SameLine();

            // ── Stats + colors + radius slider ────────────────────────────────
            ImGui::BeginChild("CL_stat", { 172.f, ph }, true);
            ImGui::TextDisabled("Cell %d  Gen %d", c.id, c.generation);
            ImGui::Text("Pos    (%.0f, %.0f)", c.position_.x, c.position_.y);
            ImGui::Text("Speed  %.3f", c.velocity_.length());
			ImGui::Text("Col Res (%.1f, %.1f)", c.collision_resolution_vector_.x, c.collision_resolution_vector_.y);
			ImGui::Text("Col ids  %d, %d", c.collision_ids.x, c.collision_ids.y);
            ImGui::Text("Radius %.1f", c.radius);
            ImGui::Text("Ate    %d  (%zu fr ago)", c.food_eaten, c.time_since_last_ate);
            ImGui::Text("Mut R  %.4f  Rng %.4f", c.mutation_rate, c.mutation_range);

            // Live friction with colour-coded value and bar
            const float fric = c.sinwave_current_friction_;
            ImVec4 fc = { 1.f - fric, fric, 0.2f, 1.f };
            ImGui::Text("Friction ");
            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Text, fc);
            ImGui::Text("%.4f", fric);
            ImGui::PopStyleColor();
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, fc);
            ImGui::ProgressBar(fric, { -1.f, 5.f }, "");
            ImGui::PopStyleColor();

            // Color swatches
            ImGui::Spacing();
            ImVec4 oc = { c.cell_outer_color.r / 255.f, c.cell_outer_color.g / 255.f,
                          c.cell_outer_color.b / 255.f, c.cell_outer_color.a / 255.f };
            ImVec4 ic = { c.cell_inner_color.r / 255.f, c.cell_inner_color.g / 255.f,
                          c.cell_inner_color.b / 255.f, c.cell_inner_color.a / 255.f };
            ImGui::ColorButton("##cout", oc, 0, { 26.f, 13.f });
            ImGui::SameLine(); ImGui::Text("Out");
            ImGui::SameLine(0, 10.f);
            ImGui::ColorButton("##cin", ic, 0, { 26.f, 13.f });
            ImGui::SameLine(); ImGui::Text("In");

            ImGui::Spacing();
            ImGui::SetNextItemWidth(-1.f);
            ImGui::SliderFloat("##rad_c", &c.radius,
                CellGenome::smallest_radius, CellGenome::largest_radius, "R=%.1f");

            ImGui::EndChild();   ImGui::SameLine();

            // ── Sinwave graph + parameter sliders ─────────────────────────────
            ImGui::BeginChild("CL_wave", { -1.f, ph }, true);
            ImGui::TextDisabled("Friction Sin Wave  A*sin(B*t+C)+D");

            static float fric_buf[120];
            fill_sinwave(fric_buf,
                c.amplitude, c.frequency, c.offset, c.vertical_shift,
                0.f, 1.f);
            const int head = (int)(p->frames_alive % 120);
            sinwave_graph("##fw", fric_buf, head, 0.f, 1.f, { -1.f, 52.f });
            ImGui::Text("t=%-3d  friction = %.4f", head, fric_buf[head]);

            // 2-per-row sliders labeled with formula letters
            ImGui::Spacing();
            const float sw = (ImGui::GetContentRegionAvail().x - 6.f) * 0.5f;

            ImGui::SetNextItemWidth(sw);
            ImGui::SliderFloat("##cA", &c.amplitude,
                -CellGenome::max_amplitude, CellGenome::max_amplitude, "A=%.3f");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(-1.f);
            ImGui::SliderFloat("##cB", &c.frequency,
                -CellGenome::max_frequency, CellGenome::max_frequency, "B=%.5f");

            ImGui::SetNextItemWidth(sw);
            ImGui::SliderFloat("##cC", &c.offset,
                -CellGenome::max_offset, CellGenome::max_offset, "C=%.3f");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(-1.f);
            ImGui::SliderFloat("##cD", &c.vertical_shift,
                -CellGenome::max_vertical_shift, CellGenome::max_vertical_shift, "D=%.3f");

            ImGui::EndChild();
        }

        ImGui::EndTabItem();
    }


    // ══════════════════════════════════════════════════════════════════════════
    //  TAB 3 — SPRINGS   (list | stats+forces+physical | wave+sliders)
    // ══════════════════════════════════════════════════════════════════════════
    if (ImGui::BeginTabItem("Springs"))
    {
        auto& springs = p->get_springs();

        if (springs.empty())
        {
            ImGui::TextDisabled("No springs.");
        }
        else
        {
            sel_spring_idx = std::min(sel_spring_idx, (int)springs.size() - 1);
            Spring& s = springs[sel_spring_idx];
            const float ph = -1.f;

            // ── Spring list ───────────────────────────────────────────────────
            ImGui::BeginChild("SL_list", { 88.f, ph }, true);
            for (int i = 0; i < (int)springs.size(); ++i)
            {
                const Spring& si = springs[i];
                if (si.broken)
                    ImGui::PushStyleColor(ImGuiCol_Text, { 1.f, 0.3f, 0.3f, 1.f });
                char lbl[32]; snprintf(lbl, sizeof(lbl), "%d->%d##%d", si.cell_A_id, si.cell_B_id, i);
                if (ImGui::Selectable(lbl, sel_spring_idx == i))
                    sel_spring_idx = i;
                if (si.broken) ImGui::PopStyleColor();
            }
            ImGui::EndChild();   ImGui::SameLine();

            // ── Stats + forces + physical sliders ─────────────────────────────
            ImGui::BeginChild("SL_stat", { 196.f, ph }, true);
            ImGui::TextDisabled("Spring %d  %d->%d",
                sel_spring_idx, s.cell_A_id, s.cell_B_id);
            if (s.broken)
            {
                ImGui::SameLine();
                ImGui::PushStyleColor(ImGuiCol_Text, { 1.f, 0.3f, 0.3f, 1.f });
                ImGui::Text("[BROKEN]");
                ImGui::PopStyleColor();
            }
            ImGui::Text("Length   %.3f", s.spring_length);
            ImGui::Text("Gen      %d", s.generation);
            ImGui::Text("Mut R    %.4f  Rng %.4f", s.mutation_rate, s.mutation_range);

            // Force vectors + magnitude bars
            ImGui::Spacing();
            ImGui::TextDisabled("Forces");
            const float mag_a = s.direction_A_force.length();
            const float mag_b = s.direction_B_force.length();
            ImGui::Text("A (%.2f, %.2f)  |%.3f|",
                s.direction_A_force.x, s.direction_A_force.y, mag_a);
            ImGui::Text("B (%.2f, %.2f)  |%.3f|",
                s.direction_B_force.x, s.direction_B_force.y, mag_b);
            constexpr float bar_sc = 0.01f;
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, { 0.4f, 0.8f, 1.f, 1.f });
            ImGui::ProgressBar(std::clamp(mag_a * bar_sc, 0.f, 1.f), { -1.f, 4.f }, "");
            ImGui::ProgressBar(std::clamp(mag_b * bar_sc, 0.f, 1.f), { -1.f, 4.f }, "");
            ImGui::PopStyleColor();

            // Physical sliders
            ImGui::Spacing();
            ImGui::TextDisabled("Physical");
            ImGui::SetNextItemWidth(-1.f);
            ImGui::SliderFloat("##sk", &s.spring_const,
                0.f, SpringGenome::max_spring_const, "K=%.3f");
            ImGui::SetNextItemWidth(-1.f);
            ImGui::SliderFloat("##sd", &s.damping,
                0.f, SpringGenome::max_damping, "D=%.3f");

            ImGui::EndChild();   ImGui::SameLine();

            // ── Sinwave graph + parameter sliders ─────────────────────────────
            ImGui::BeginChild("SL_wave", { -1.f, ph }, true);
            ImGui::TextDisabled("Extension Sin Wave  rest-length ratio [0,1]");

            static float ext_buf[120];
            fill_sinwave(ext_buf,
                s.amplitude, s.frequency, s.offset, s.vertical_shift,
                0.f, 1.f);
            const int head = (int)(p->frames_alive % 120);
            sinwave_graph("##sw", ext_buf, head, 0.f, 1.f, { -1.f, 52.f });
            ImGui::Text("t=%-3d  ratio = %.4f", head, ext_buf[head]);

            ImGui::Spacing();
            const float sw = (ImGui::GetContentRegionAvail().x - 6.f) * 0.5f;

            ImGui::SetNextItemWidth(sw);
            ImGui::SliderFloat("##sA", &s.amplitude,
                0.f, SpringGenome::max_amplitude, "A=%.3f");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(-1.f);
            ImGui::SliderFloat("##sB", &s.frequency,
                -SpringGenome::max_frequency, SpringGenome::max_frequency, "B=%.5f");

            ImGui::SetNextItemWidth(sw);
            ImGui::SliderFloat("##sC", &s.offset,
                -SpringGenome::max_offset, SpringGenome::max_offset, "C=%.3f");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(-1.f);
            ImGui::SliderFloat("##sD", &s.vertical_shift,
                -SpringGenome::max_vertical_shift, SpringGenome::max_vertical_shift, "D=%.3f");

            ImGui::EndChild();
        }

        ImGui::EndTabItem();
    }


    // ══════════════════════════════════════════════════════════════════════════
    //  TAB 4 — TUNING   (3 equal columns)
    // ══════════════════════════════════════════════════════════════════════════
    if (ImGui::BeginTabItem("Tuning"))
    {
        auto& cells = p->get_cells();
        auto& springs = p->get_springs();

        const float cw = (ImGui::GetContentRegionAvail().x - 18.f) / 3.f;
        const float ch = -1.f;

        // ── Col 1: Mutation controls ──────────────────────────────────────────
        ImGui::BeginChild("TN_mut", { cw, ch }, true);
        ImGui::TextDisabled("Mutation");
        static float tun_rate = 0.2f;
        static float tun_range = 0.2f;
        ImGui::SetNextItemWidth(-1.f);
        ImGui::SliderFloat("##tr", &tun_rate, 0.f, 1.f, "rate  = %.3f");
        ImGui::SetNextItemWidth(-1.f);
        ImGui::SliderFloat("##trng", &tun_range, 0.f, 1.f, "range = %.3f");
        ImGui::Spacing();
        if (ImGui::Button("Apply Mutation", { -1.f, 0.f }))
            p->mutate(false, tun_rate, tun_range);
        ImGui::Spacing();
        ImGui::TextDisabled("Structure");
        if (ImGui::Button("Add Cell", { -1.f, 0.f }))
            p->mutate(true, 0.f, 0.f);
        ImGui::EndChild();   ImGui::SameLine();

        // ── Col 2: Live genome averages ───────────────────────────────────────
        ImGui::BeginChild("TN_avg", { cw, ch }, true);
        ImGui::TextDisabled("Live Genome Averages");
        if (!cells.empty())
        {
            float sAmp = 0.f, sFreq = 0.f, sMut = 0.f, sRng = 0.f, sRad = 0.f;
            for (const Cell& c : cells)
            {
                sAmp += c.amplitude;
                sFreq += c.frequency;
                sMut += c.mutation_rate;
                sRng += c.mutation_range;
                sRad += c.radius;
            }
            const float n = (float)cells.size();
            ImGui::Text("Cells   %d", (int)cells.size());
            ImGui::Text("Amp     %.4f", sAmp / n);
            ImGui::Text("Freq    %.6f", sFreq / n);
            ImGui::Text("Mut R   %.4f", sMut / n);
            ImGui::Text("Mut Rng %.4f", sRng / n);
            ImGui::Text("Radius  %.2f", sRad / n);
        }
        if (!springs.empty())
        {
            float sk = 0.f, sd = 0.f;
            for (const Spring& s : springs) { sk += s.spring_const; sd += s.damping; }
            const float n = (float)springs.size();
            ImGui::Spacing();
            ImGui::Text("Springs %d", (int)springs.size());
            ImGui::Text("Spr K   %.4f", sk / n);
            ImGui::Text("Spr D   %.4f", sd / n);
        }
        ImGui::EndChild();   ImGui::SameLine();

        // ── Col 3: Danger zone ────────────────────────────────────────────────
        ImGui::BeginChild("TN_danger", { -1.f, ch }, true);
        ImGui::TextDisabled("Danger Zone");
        ImGui::Spacing();
        ImGui::PushStyleColor(ImGuiCol_Button, { 0.55f, 0.08f, 0.08f, 1.f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.75f, 0.15f, 0.15f, 1.f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 1.00f, 0.25f, 0.25f, 1.f });
        if (ImGui::Button("Kill Protozoa", { -1.f, 0.f }))
        {
            p->kill();
            m_world_.deselect_protozoa();
        }
        ImGui::PopStyleColor(3);
        ImGui::EndChild();

        ImGui::EndTabItem();
    }


    ImGui::EndTabBar();
    ImGui::End();
    ImGui::PopStyleVar(); // ItemSpacing
}