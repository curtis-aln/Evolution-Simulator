#include "organism_tab.h"
#include "../helpers/plot_utils.h"
#include "../helpers/confirm_button.h"
#include <imgui.h>
#include <algorithm>
#include <cmath>

// ─────────────────────────────────────────────────────────────────────────────
//  Internal helpers
// ─────────────────────────────────────────────────────────────────────────────

// Hard cap on sinwave buffer to prevent OOM when frequency is near zero.
static constexpr int k_max_wave_buf = 2048;

// Safe period in frames for a given frequency.
static int safe_time_period(float frequency)
{
    if (std::abs(frequency) < 1e-6f) return 120;
    return std::clamp(static_cast<int>(1.f / std::abs(frequency)), 1, k_max_wave_buf);
}

// Analytical min/max of A*sin(...)+D clamped to [lo, hi].
// sin ranges over [-1, 1] so the wave spans [D-|A|, D+|A|].
static void wave_range(float A, float D, float lo, float hi,
    float& out_min, float& out_max)
{
    out_min = std::clamp(D - std::abs(A), lo, hi);
    out_max = std::clamp(D + std::abs(A), lo, hi);
}

// Green-to-red gradient: green at f=1, red at f=0.
static ImVec4 fraction_color(float f)
{
    return f > 0.5f ? ImVec4{ 2.f * (1.f - f), 1.f, 0.2f, 1.f }
    : ImVec4{ 1.f, 2.f * f,     0.2f, 1.f };
}

static void colored_progress(float fraction, ImVec4 color,
    const char* label, ImVec2 size = { -1.f, 10.f })
{
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, color);
    ImGui::ProgressBar(fraction, size, label);
    ImGui::PopStyleColor();
}

// ─────────────────────────────────────────────────────────────────────────────
//  Top-level draw
// ─────────────────────────────────────────────────────────────────────────────
void OrganismTab::draw(UIContext& ctx)
{
    Protozoa* p = ctx.world.get_selected_protozoa();
    if (!p) { draw_no_selection(); return; }

    if (p->id != m_last_id_)
    {
        m_last_id_ = p->id;
        m_sel_cell_idx_ = 0;
        m_sel_spring_idx_ = 0;
        m_sel_is_spring_ = false;
    }

    ImGui::BeginChild("OV_panel", { 240.f, -1.f }, false);
    draw_overview(p);
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("TAB_panel", { -1.f, -1.f }, false);
    if (!ImGui::BeginTabBar("##org_tabs")) { ImGui::EndChild(); return; }

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 6.f, 2.f });
    if (ImGui::BeginTabItem("Cells & Springs")) { draw_cells_springs_tab(p);        ImGui::EndTabItem(); }
    if (ImGui::BeginTabItem("Tuning & Controls")) { draw_tuning_controls_tab(ctx, p); ImGui::EndTabItem(); }
    ImGui::PopStyleVar();

    ImGui::EndTabBar();
    ImGui::EndChild();
}

// ─────────────────────────────────────────────────────────────────────────────
//  No selection
// ─────────────────────────────────────────────────────────────────────────────
void OrganismTab::draw_no_selection()
{
    ImGui::Spacing();
    const char* msg = "No organism selected — click one in the world";
    ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(msg).x) * 0.5f);
    ImGui::TextDisabled("%s", msg);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Overview panel
// ─────────────────────────────────────────────────────────────────────────────
void OrganismTab::draw_overview(Protozoa* p)
{
    // ── Identity / Locomotion side-by-side ───────────────────────────────
    ImGui::Columns(2, nullptr, false);

    ImGui::TextDisabled("Identity");
    ImGui::Text("ID      %d", p->id);
    ImGui::Text("Age     %u", p->frames_alive);
    ImGui::Text("Cells   %d", (int)p->get_cells().size());
    ImGui::Text("Springs %d", (int)p->get_springs().size());
    ImGui::Text("Birth (%.0f,%.0f)", p->birth_location.x, p->birth_location.y);

    ImGui::NextColumn();

    ImGui::TextDisabled("Locomotion");
    ImGui::Text("Speed %.4f", p->velocity.length());
    ImGui::Text("Vel X %.3f", p->velocity.x);
    ImGui::Text("Vel Y %.3f", p->velocity.y);
    ImGui::Spacing();
    ImGui::TextDisabled("Offspring");
    ImGui::Text("Count %d", p->offspring_count);
    ImGui::Text("Food  %u", p->total_food_eaten);

    ImGui::Columns(1);
    ImGui::Spacing();

    // ── Energy ───────────────────────────────────────────────────────────
    ImGui::TextDisabled("Energy");
    const float energy_f = std::clamp(p->get_energy() / ProtozoaSettings::initial_energy, 0.f, 1.f);
    char energy_lbl[32];
    snprintf(energy_lbl, sizeof(energy_lbl), "%.1f / %.0f", p->get_energy(), ProtozoaSettings::initial_energy);
    colored_progress(energy_f, fraction_color(energy_f), energy_lbl);
    ImGui::Text("Spr cost %.4f", p->energy_lost_to_springs);

    // ── Stomach (discrete food count toward reproduction threshold) ───────
    ImGui::Spacing();
    ImGui::TextDisabled("Stomach");
    const int   food_held = p->stomach_capacity();
    const int   food_thresh = p->stomach_reproduce_thresh();
    const float stomach_f = std::clamp((float)food_held / (float)food_thresh, 0.f, 1.f);
    char stomach_lbl[24];
    snprintf(stomach_lbl, sizeof(stomach_lbl), "%d / %d", food_held, food_thresh);
    colored_progress(stomach_f, { 0.35f, 0.75f, 0.35f, 1.f }, stomach_lbl);

    // ── Repro cooldown: counts down to zero, stays at zero when ready ─────
    ImGui::Spacing();
    ImGui::TextDisabled("Repro cooldown");
    const float cooldown = static_cast<float>(ProtozoaSettings::reproductive_cooldown);
    const float elapsed = static_cast<float>(p->time_since_last_reproduced);
    const float remaining_f = std::clamp(1.f - elapsed / cooldown, 0.f, 1.f);
    char repro_lbl[24];
    snprintf(repro_lbl, sizeof(repro_lbl), "%.0f fr left", remaining_f * cooldown);
    colored_progress(remaining_f, { 0.6f, 0.4f, 0.9f, 1.f },
        remaining_f <= 0.f ? "Ready" : repro_lbl);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Cells & Springs tab
// ─────────────────────────────────────────────────────────────────────────────
void OrganismTab::draw_cells_springs_tab(Protozoa* p)
{
    auto& cells = p->get_cells();
    auto& springs = p->get_springs();

    if (cells.empty() && springs.empty()) { ImGui::TextDisabled("No cells or springs."); return; }

    if (!cells.empty())   m_sel_cell_idx_ = std::min(m_sel_cell_idx_, (int)cells.size() - 1);
    if (!springs.empty()) m_sel_spring_idx_ = std::min(m_sel_spring_idx_, (int)springs.size() - 1);

    if (m_sel_is_spring_ && springs.empty()) m_sel_is_spring_ = false;
    if (!m_sel_is_spring_ && cells.empty())   m_sel_is_spring_ = true;

    // ── Unified selection list ────────────────────────────────────────────
    ImGui::BeginChild("CS_list", { 88.f, -1.f }, true);

    for (int i = 0; i < (int)cells.size(); ++i)
    {
        const Cell& ci = cells[i];
        const ImVec4 dot = { ci.cell_outer_color.r / 255.f,
                             ci.cell_outer_color.g / 255.f,
                             ci.cell_outer_color.b / 255.f, 1.f };
        ImGui::PushStyleColor(ImGuiCol_Text, dot);
        ImGui::Text("●");
        ImGui::PopStyleColor();
        ImGui::SameLine();
        char lbl[12]; snprintf(lbl, sizeof(lbl), "C%d", i);
        if (ImGui::Selectable(lbl, !m_sel_is_spring_ && m_sel_cell_idx_ == i))
        {
            m_sel_cell_idx_ = i;
            m_sel_is_spring_ = false;
        }
    }

    if (!springs.empty())
    {
        ImGui::Separator();
        for (int i = 0; i < (int)springs.size(); ++i)
        {
            const Spring& si = springs[i];
            if (si.broken) ImGui::PushStyleColor(ImGuiCol_Text, { 1.f, 0.3f, 0.3f, 1.f });
            char lbl[32]; snprintf(lbl, sizeof(lbl), "%d->%d##sp%d", si.cell_A_id, si.cell_B_id, i);
            if (ImGui::Selectable(lbl, m_sel_is_spring_ && m_sel_spring_idx_ == i))
            {
                m_sel_spring_idx_ = i;
                m_sel_is_spring_ = true;
            }
            if (si.broken) ImGui::PopStyleColor();
        }
    }

    ImGui::EndChild();
    ImGui::SameLine();

    if (!m_sel_is_spring_ && !cells.empty())
        draw_cell_detail(p, cells[m_sel_cell_idx_]);
    else if (m_sel_is_spring_ && !springs.empty())
        draw_spring_detail(p, springs[m_sel_spring_idx_]);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Cell detail (stats + sinwave)
// ─────────────────────────────────────────────────────────────────────────────
void OrganismTab::draw_cell_detail(Protozoa* p, Cell& c)
{
    const int period = safe_time_period(c.frequency);
    const int display_size = std::min(m_wave_cycles_ * period, k_max_wave_buf);
    const int head = static_cast<int>(p->frames_alive % display_size);

    float wave_min, wave_max;
    wave_range(c.amplitude, c.vertical_shift, 0.f, 1.f, wave_min, wave_max);

    // ── Stats ─────────────────────────────────────────────────────────────
    ImGui::BeginChild("CL_stat", { 230.f, -1.f }, true);

    ImGui::TextDisabled("Cell %d  Gen %d", c.id, c.generation);
    ImGui::Text("Pos      (%.0f, %.0f)", c.position_.x, c.position_.y);
    ImGui::Text("Speed    %.3f", c.velocity_.length());
    ImGui::Text("Radius   %.1f", c.radius);
    ImGui::Text("Period   %d fr", period);
    ImGui::Text("Fric     min %.3f  max %.3f", wave_min, wave_max);
    ImGui::Text("Mut R    %.4f  Rng %.4f", c.mutation_rate, c.mutation_range);
    ImGui::Text("Ate      %d  (%zu fr ago)", c.food_eaten, c.time_since_last_ate);

    // Digest cooldown bar
    const float digest_remaining = std::max(0.f,
        static_cast<float>(ProtozoaSettings::digestive_time) -
        static_cast<float>(c.time_since_last_ate));
    const float digest_f = digest_remaining / static_cast<float>(ProtozoaSettings::digestive_time);
    char digest_lbl[32];
    snprintf(digest_lbl, sizeof(digest_lbl), "%.0f fr left", digest_remaining);
    ImGui::TextDisabled("Digest cooldown");
    colored_progress(digest_f, fraction_color(digest_f),
        digest_f <= 0.f ? "Ready" : digest_lbl);

    // Current friction
    const float fric = c.sinwave_current_friction_;
    const ImVec4 fc = { 1.f - fric, fric, 0.2f, 1.f };
    ImGui::Spacing();
    ImGui::TextDisabled("Friction");
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Text, fc);
    ImGui::Text("%.4f", fric);
    ImGui::PopStyleColor();
    colored_progress(fric, fc, "", { -1.f, 5.f });

    // Color swatches + radius slider
    ImGui::Spacing();
    const ImVec4 oc = { c.cell_outer_color.r / 255.f, c.cell_outer_color.g / 255.f,
                        c.cell_outer_color.b / 255.f, c.cell_outer_color.a / 255.f };
    const ImVec4 ic = { c.cell_inner_color.r / 255.f, c.cell_inner_color.g / 255.f,
                        c.cell_inner_color.b / 255.f, c.cell_inner_color.a / 255.f };
    ImGui::ColorButton("##cout", oc, 0, { 26.f, 13.f }); ImGui::SameLine(); ImGui::Text("Out");
    ImGui::SameLine(0, 10.f);
    ImGui::ColorButton("##cin", ic, 0, { 26.f, 13.f }); ImGui::SameLine(); ImGui::Text("In");
    ImGui::Spacing();
    ImGui::SetNextItemWidth(-1.f);
    ImGui::SliderFloat("##rad_c", &c.radius,
        CellGenome::smallest_radius, CellGenome::largest_radius, "R = %.1f");

    ImGui::EndChild();
    ImGui::SameLine();

    // ── Sinwave ───────────────────────────────────────────────────────────
    ImGui::BeginChild("CL_wave", { -1.f, -1.f }, true);
    ImGui::TextDisabled("Friction  amplitude * sin(frequency * t + phase) + shift");

    static std::vector<float> fric_buf;
    fric_buf.resize(display_size);
    PlotUtils::fill_sinwave(fric_buf.data(), display_size,
        c.amplitude, c.frequency, c.offset, c.vertical_shift, 0.f, 1.f);
    PlotUtils::sinwave_graph("##fw", fric_buf.data(), display_size, head, 0.f, 1.f, { -1.f, 52.f });
    ImGui::Text("t=%-4d  friction = %.4f", head, fric_buf[head]);

    ImGui::Spacing();
    ImGui::SetNextItemWidth(-1.f);
    ImGui::SliderInt("##cycles_c", &m_wave_cycles_, 1, 8, "Display cycles = %d");

    ImGui::Spacing();
    ImGui::SetNextItemWidth(-1.f);
    ImGui::SliderFloat("##cA", &c.amplitude, -CellGenome::max_amplitude, CellGenome::max_amplitude, "Amplitude = %.3f");
    ImGui::SetNextItemWidth(-1.f);
    ImGui::SliderFloat("##cB", &c.frequency, -CellGenome::max_frequency, CellGenome::max_frequency, "Frequency = %.5f");
    ImGui::SetNextItemWidth(-1.f);
    ImGui::SliderFloat("##cC", &c.offset, -CellGenome::max_offset, CellGenome::max_offset, "Phase     = %.3f");
    ImGui::SetNextItemWidth(-1.f);
    ImGui::SliderFloat("##cD", &c.vertical_shift, -CellGenome::max_vertical_shift, CellGenome::max_vertical_shift, "Shift     = %.3f");

    ImGui::EndChild();
}

// ─────────────────────────────────────────────────────────────────────────────
//  Spring detail (stats + sinwave)
// ─────────────────────────────────────────────────────────────────────────────
void OrganismTab::draw_spring_detail(Protozoa* p, Spring& s)
{
    const int period = safe_time_period(s.frequency);
    const int display_size = std::min(m_wave_cycles_ * period, k_max_wave_buf);
    const int head = static_cast<int>(p->frames_alive % display_size);

    float ext_min, ext_max;
    wave_range(s.amplitude, s.vertical_shift, 0.f, 1.f, ext_min, ext_max);

    const float ma = s.direction_A_force.length();
    const float mb = s.direction_B_force.length();

    // ── Stats ─────────────────────────────────────────────────────────────
    ImGui::BeginChild("SL_stat", { 230.f, -1.f }, true);

    ImGui::TextDisabled("Spring %d->%d", s.cell_A_id, s.cell_B_id);
    if (s.broken)
    {
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, { 1.f, 0.3f, 0.3f, 1.f });
        ImGui::Text("[BROKEN]");
        ImGui::PopStyleColor();
    }
    ImGui::Text("Rest length  %.3f", s.spring_length);
    ImGui::Text("Period       %d fr", period);
    ImGui::Text("Ext min      %.3f  max %.3f", ext_min, ext_max);
    ImGui::Text("Gen          %d", s.generation);
    ImGui::Text("Mut R        %.4f  Rng %.4f", s.mutation_rate, s.mutation_range);

    ImGui::Spacing();
    ImGui::TextDisabled("Forces");
    ImGui::Text("A (%.2f, %.2f)  |%.3f|", s.direction_A_force.x, s.direction_A_force.y, ma);
    ImGui::Text("B (%.2f, %.2f)  |%.3f|", s.direction_B_force.x, s.direction_B_force.y, mb);
    const float force_scale = SpringGenome::max_spring_const > 0.f
        ? 1.f / SpringGenome::max_spring_const : 1.f;
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, { 0.4f, 0.8f, 1.f, 1.f });
    ImGui::ProgressBar(std::clamp(ma * force_scale, 0.f, 1.f), { -1.f, 4.f }, "");
    ImGui::ProgressBar(std::clamp(mb * force_scale, 0.f, 1.f), { -1.f, 4.f }, "");
    ImGui::PopStyleColor();

    ImGui::Spacing();
    ImGui::TextDisabled("Physical");
    ImGui::SetNextItemWidth(-1.f);
    ImGui::SliderFloat("##sk", &s.spring_const, 0.f, SpringGenome::max_spring_const, "Spring constant = %.3f");
    ImGui::SetNextItemWidth(-1.f);
    ImGui::SliderFloat("##sd", &s.damping, 0.f, SpringGenome::max_damping, "Damping         = %.3f");

    ImGui::EndChild();
    ImGui::SameLine();

    // ── Sinwave ───────────────────────────────────────────────────────────
    ImGui::BeginChild("SL_wave", { -1.f, -1.f }, true);
    ImGui::TextDisabled("Extension  amplitude * sin(frequency * t + phase) + shift  [0, 1]");

    static std::vector<float> ext_buf;
    ext_buf.resize(display_size);
    PlotUtils::fill_sinwave(ext_buf.data(), display_size,
        s.amplitude, s.frequency, s.offset, s.vertical_shift, 0.f, 1.f);
    PlotUtils::sinwave_graph("##sw", ext_buf.data(), display_size, head, 0.f, 1.f, { -1.f, 52.f });
    ImGui::Text("t=%-4d  ratio = %.4f", head, ext_buf[head]);

    ImGui::Spacing();
    ImGui::SetNextItemWidth(-1.f);
    ImGui::SliderInt("##cycles_s", &m_wave_cycles_, 1, 8, "Display cycles = %d");

    ImGui::Spacing();
    ImGui::SetNextItemWidth(-1.f);
    ImGui::SliderFloat("##sA", &s.amplitude, 0.f, SpringGenome::max_amplitude, "Amplitude = %.3f");
    ImGui::SetNextItemWidth(-1.f);
    ImGui::SliderFloat("##sB", &s.frequency, -SpringGenome::max_frequency, SpringGenome::max_frequency, "Frequency = %.5f");
    ImGui::SetNextItemWidth(-1.f);
    ImGui::SliderFloat("##sC", &s.offset, -SpringGenome::max_offset, SpringGenome::max_offset, "Phase     = %.3f");
    ImGui::SetNextItemWidth(-1.f);
    ImGui::SliderFloat("##sD", &s.vertical_shift, -SpringGenome::max_vertical_shift, SpringGenome::max_vertical_shift, "Shift     = %.3f");

    ImGui::EndChild();
}

// ─────────────────────────────────────────────────────────────────────────────
//  Tuning & Controls tab
// ─────────────────────────────────────────────────────────────────────────────
void OrganismTab::draw_tuning_controls_tab(UIContext& ctx, Protozoa* p)
{
    const float half = (ImGui::GetContentRegionAvail().x - 8.f) * 0.5f;

    // ── Left: mutation, structure, feed ──────────────────────────────────
    ImGui::BeginChild("TC_left", { half, -1.f }, false);

    ImGui::BeginChild("TC_mut", { -1.f, 0.f }, true);
    ImGui::TextDisabled("Mutation");
    static float tun_rate = 0.2f, tun_range = 0.2f;
    ImGui::SetNextItemWidth(-1.f); ImGui::SliderFloat("##tr", &tun_rate, 0.f, 1.f, "rate  = %.3f");
    ImGui::SetNextItemWidth(-1.f); ImGui::SliderFloat("##trng", &tun_range, 0.f, 1.f, "range = %.3f");
    ImGui::Spacing();
    if (ImGui::Button("Apply Mutation", { -1.f, 0.f })) p->mutate(false, tun_rate, tun_range);
    ImGui::Spacing();
    ImGui::TextDisabled("Structure");
    ImGui::Columns(2, nullptr, false);
    if (ImGui::Button("Add Cell", { -1.f, 0.f })) p->mutate(true, 0.f, 0.f);
    if (ImGui::Button("Remove Cell", { -1.f, 0.f })) p->remove_cell();
    ImGui::NextColumn();
    if (ImGui::Button("Add Spring", { -1.f, 0.f })) p->add_spring();
    if (ImGui::Button("Remove Spring", { -1.f, 0.f })) p->remove_spring();
    ImGui::Columns(1);
    ImGui::EndChild();

    ImGui::Spacing();

    // Feed lives in the left column so it's always visible without scrolling
    ImGui::BeginChild("TC_feed", { -1.f, 0.f }, true);
    ImGui::TextDisabled("Feed");
    static float feed_energy = 50.f;
    ImGui::SetNextItemWidth(-70.f);
    ImGui::SliderFloat("##feed", &feed_energy, 1.f, 500.f, "%.0f energy");
    ImGui::SameLine();
    if (ImGui::Button("Inject##org")) p->inject(feed_energy);
    ImGui::EndChild();

    ImGui::EndChild(); // TC_left
    ImGui::SameLine();

    // ── Right: lifecycle, clone/file, tag ────────────────────────────────
    ImGui::BeginChild("TC_right", { -1.f, -1.f }, false);

    ImGui::BeginChild("TC_life", { -1.f, 0.f }, true);
    ImGui::TextDisabled("Lifecycle");
    ImGui::Checkbox("Immortal##org", &p->immortal);
    ImGui::Spacing();
    if (ImGui::Button("Force Reproduce##org", { -1.f, 0.f })) p->force_reproduce();
    ImGui::Spacing();
    ImGui::PushStyleColor(ImGuiCol_Button, { 0.55f, 0.08f, 0.08f, 1.f });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.75f, 0.15f, 0.15f, 1.f });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 1.00f, 0.25f, 0.25f, 1.f });
    if (ConfirmButton::draw("Force Die##org", { -1.f, 0.f }))
    {
        p->kill();
        ctx.world.deselect_protozoa();
    }
    ImGui::PopStyleColor(3);
    ImGui::EndChild();

    ImGui::Spacing();

    ImGui::BeginChild("TC_clone", { -1.f, 0.f }, true);
    ImGui::TextDisabled("Clone & File");
    if (ImGui::Button("Clone nearby##org", { -1.f, 0.f })) ctx.world.create_offspring(p, false);
    if (ImGui::Button("Save to file (stub)##org", { -1.f, 0.f })) {}
    if (ImGui::Button("Load & spawn from file (stub)##org", { -1.f, 0.f })) {}
    ImGui::EndChild();

    ImGui::Spacing();

    ImGui::BeginChild("TC_tag", { -1.f, 0.f }, true);
    ImGui::TextDisabled("Tag");
    ImGui::TextDisabled("Use the Tagged tab to tag ID %d", p->id);
    ImGui::EndChild();

    ImGui::EndChild(); // TC_right
}