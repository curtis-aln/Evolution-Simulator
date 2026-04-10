#include "organism_tab.h"
#include "../helpers/plot_utils.h"
#include "../helpers/confirm_button.h"
#include <imgui.h>
#include <algorithm>
#include <cmath>

static constexpr float k_init_energy = 300.f;

void OrganismTab::draw(UIContext& ctx)
{
    Protozoa* p = ctx.world.get_selected_protozoa();
    if (!p) { draw_no_selection(); return; }

    if (p->id != m_last_id_)
    {
        m_last_id_ = p->id;
        m_sel_cell_idx_ = 0;
        m_sel_spring_idx_ = 0;
    }

    if (!ImGui::BeginTabBar("##org_tabs")) return;

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 6.f, 2.f });

    if (ImGui::BeginTabItem("Overview")) { draw_overview(p);          ImGui::EndTabItem(); }
    if (ImGui::BeginTabItem("Cells")) { draw_cells_tab(p);         ImGui::EndTabItem(); }
    if (ImGui::BeginTabItem("Springs")) { draw_springs_tab(p);       ImGui::EndTabItem(); }
    if (ImGui::BeginTabItem("Tuning")) { draw_tuning_tab(p);        ImGui::EndTabItem(); }
    if (ImGui::BeginTabItem("Controls")) { draw_controls_tab(ctx, p); ImGui::EndTabItem(); }

    ImGui::PopStyleVar();
    ImGui::EndTabBar();
}

void OrganismTab::draw_no_selection()
{
    ImGui::Spacing();
    const char* msg = "No organism selected — click one in the world";
    ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(msg).x) * 0.5f);
    ImGui::TextDisabled("%s", msg);
}

void OrganismTab::draw_overview(Protozoa* p)
{
    const float cw = (ImGui::GetContentRegionAvail().x - 18.f) / 4.f;

    ImGui::BeginChild("OV_id", { cw, -1.f }, true);
    ImGui::TextDisabled("Identity");
    ImGui::Text("ID       %d", p->id);
    ImGui::Text("Age      %u fr", p->frames_alive);
    ImGui::Text("Cells    %d", (int)p->get_cells().size());
    ImGui::Text("Springs  %d", (int)p->get_springs().size());
    ImGui::Text("Birth    (%.0f, %.0f)", p->birth_location.x, p->birth_location.y);
    ImGui::EndChild(); ImGui::SameLine();

    ImGui::BeginChild("OV_en", { cw, -1.f }, true);
    ImGui::TextDisabled("Energy");
    const float ef = std::clamp(p->get_energy() / k_init_energy, 0.f, 1.f);
    ImVec4 bc = ef > 0.5f
        ? ImVec4{ 2.f * (1.f - ef), 1.f,     0.2f, 1.f }
    : ImVec4{ 1.f,              2.f * ef, 0.2f, 1.f };
    char elbl[32];
    snprintf(elbl, sizeof(elbl), "%.1f / %.0f", p->get_energy(), k_init_energy);
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, bc);
    ImGui::ProgressBar(ef, { -1.f, 10.f }, elbl);
    ImGui::PopStyleColor();
    ImGui::Text("Spring cost  %.4f", p->energy_lost_to_springs);
    ImGui::EndChild(); ImGui::SameLine();

    ImGui::BeginChild("OV_loc", { cw, -1.f }, true);
    ImGui::TextDisabled("Locomotion");
    ImGui::Text("Speed  %.4f", p->velocity.length());
    ImGui::Text("Vel X  %.3f", p->velocity.x);
    ImGui::Text("Vel Y  %.3f", p->velocity.y);
    ImGui::EndChild(); ImGui::SameLine();

    ImGui::BeginChild("OV_rep", { -1.f, -1.f }, true);
    ImGui::TextDisabled("Reproduction");
    ImGui::Text("Offspring    %d", p->offspring_count);
    ImGui::Text("Food eaten   %u", p->total_food_eaten);
    ImGui::Text("Since repr   %zu fr", p->time_since_last_reproduced);
    ImGui::EndChild();
}

void OrganismTab::draw_cells_tab(Protozoa* p)
{
    auto& cells = p->get_cells();
    if (cells.empty()) { ImGui::TextDisabled("No cells."); return; }

    m_sel_cell_idx_ = std::min(m_sel_cell_idx_, (int)cells.size() - 1);
    Cell& c = cells[m_sel_cell_idx_];

    // ── Cell list ────────────────────────────────────────────────────────
    ImGui::BeginChild("CL_list", { 78.f, -1.f }, true);
    for (int i = 0; i < (int)cells.size(); ++i)
    {
        ImVec4 dot = { cells[i].cell_outer_color.r / 255.f,
                       cells[i].cell_outer_color.g / 255.f,
                       cells[i].cell_outer_color.b / 255.f, 1.f };
        ImGui::PushStyleColor(ImGuiCol_Text, dot);
        ImGui::Text("●");
        ImGui::PopStyleColor();
        ImGui::SameLine();
        char lbl[12]; snprintf(lbl, sizeof(lbl), "C%d", i);
        if (ImGui::Selectable(lbl, m_sel_cell_idx_ == i))
            m_sel_cell_idx_ = i;
    }
    ImGui::EndChild(); ImGui::SameLine();

    // ── Stats ────────────────────────────────────────────────────────────
    ImGui::BeginChild("CL_stat", { 172.f, -1.f }, true);
    ImGui::TextDisabled("Cell %d  Gen %d", c.id, c.generation);
    ImGui::Text("Pos    (%.0f, %.0f)", c.position_.x, c.position_.y);
    ImGui::Text("Speed  %.3f", c.velocity_.length());
    ImGui::Text("Radius %.1f", c.radius);
    ImGui::Text("Ate    %d  (%zu fr ago)", c.food_eaten, c.time_since_last_ate);
    ImGui::Text("Mut R  %.4f  Rng %.4f", c.mutation_rate, c.mutation_range);

    const float fric = c.sinwave_current_friction_;
    ImVec4 fc = { 1.f - fric, fric, 0.2f, 1.f };
    ImGui::Text("Friction "); ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Text, fc);          ImGui::Text("%.4f", fric); ImGui::PopStyleColor();
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, fc); ImGui::ProgressBar(fric, { -1.f, 5.f }, ""); ImGui::PopStyleColor();

    ImGui::Spacing();
    ImVec4 oc = { c.cell_outer_color.r / 255.f, c.cell_outer_color.g / 255.f,
                  c.cell_outer_color.b / 255.f, c.cell_outer_color.a / 255.f };
    ImVec4 ic = { c.cell_inner_color.r / 255.f, c.cell_inner_color.g / 255.f,
                  c.cell_inner_color.b / 255.f, c.cell_inner_color.a / 255.f };
    ImGui::ColorButton("##cout", oc, 0, { 26.f, 13.f }); ImGui::SameLine(); ImGui::Text("Out");
    ImGui::SameLine(0, 10.f);
    ImGui::ColorButton("##cin", ic, 0, { 26.f, 13.f }); ImGui::SameLine(); ImGui::Text("In");
    ImGui::Spacing();
    ImGui::SetNextItemWidth(-1.f);
    ImGui::SliderFloat("##rad_c", &c.radius, CellGenome::smallest_radius, CellGenome::largest_radius, "R=%.1f");
    ImGui::EndChild(); ImGui::SameLine();

    // ── Sinwave ──────────────────────────────────────────────────────────
    ImGui::BeginChild("CL_wave", { -1.f, -1.f }, true);
    ImGui::TextDisabled("Friction Sin Wave  A*sin(B*t+C)+D");
    static float fric_buf[120];
    PlotUtils::fill_sinwave(fric_buf, c.amplitude, c.frequency, c.offset, c.vertical_shift, 0.f, 1.f);
    const int head = (int)(p->frames_alive % 120);
    PlotUtils::sinwave_graph("##fw", fric_buf, head, 0.f, 1.f, { -1.f, 52.f });
    ImGui::Text("t=%-3d  friction = %.4f", head, fric_buf[head]);

    ImGui::Spacing();
    const float sw = (ImGui::GetContentRegionAvail().x - 6.f) * 0.5f;
    ImGui::SetNextItemWidth(sw);  ImGui::SliderFloat("##cA", &c.amplitude, -CellGenome::max_amplitude, CellGenome::max_amplitude, "A=%.3f");
    ImGui::SameLine(); ImGui::SetNextItemWidth(-1.f);
    ImGui::SliderFloat("##cB", &c.frequency, -CellGenome::max_frequency, CellGenome::max_frequency, "B=%.5f");
    ImGui::SetNextItemWidth(sw);  ImGui::SliderFloat("##cC", &c.offset, -CellGenome::max_offset, CellGenome::max_offset, "C=%.3f");
    ImGui::SameLine(); ImGui::SetNextItemWidth(-1.f);
    ImGui::SliderFloat("##cD", &c.vertical_shift, -CellGenome::max_vertical_shift, CellGenome::max_vertical_shift, "D=%.3f");
    ImGui::EndChild();
}

void OrganismTab::draw_springs_tab(Protozoa* p)
{
    auto& springs = p->get_springs();
    if (springs.empty()) { ImGui::TextDisabled("No springs."); return; }

    m_sel_spring_idx_ = std::min(m_sel_spring_idx_, (int)springs.size() - 1);
    Spring& s = springs[m_sel_spring_idx_];

    // ── List ─────────────────────────────────────────────────────────────
    ImGui::BeginChild("SL_list", { 88.f, -1.f }, true);
    for (int i = 0; i < (int)springs.size(); ++i)
    {
        const Spring& si = springs[i];
        if (si.broken) ImGui::PushStyleColor(ImGuiCol_Text, { 1.f, 0.3f, 0.3f, 1.f });
        char lbl[32]; snprintf(lbl, sizeof(lbl), "%d->%d##%d", si.cell_A_id, si.cell_B_id, i);
        if (ImGui::Selectable(lbl, m_sel_spring_idx_ == i)) m_sel_spring_idx_ = i;
        if (si.broken) ImGui::PopStyleColor();
    }
    ImGui::EndChild(); ImGui::SameLine();

    // ── Stats ────────────────────────────────────────────────────────────
    ImGui::BeginChild("SL_stat", { 196.f, -1.f }, true);
    ImGui::TextDisabled("Spring %d  %d->%d", m_sel_spring_idx_, s.cell_A_id, s.cell_B_id);
    if (s.broken)
    {
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, { 1.f, 0.3f, 0.3f, 1.f });
        ImGui::Text("[BROKEN]");
        ImGui::PopStyleColor();
    }
    ImGui::Text("Length  %.3f", s.spring_length);
    ImGui::Text("Gen     %d", s.generation);
    ImGui::Text("Mut R   %.4f  Rng %.4f", s.mutation_rate, s.mutation_range);

    ImGui::Spacing(); ImGui::TextDisabled("Forces");
    const float ma = s.direction_A_force.length();
    const float mb = s.direction_B_force.length();
    ImGui::Text("A (%.2f, %.2f)  |%.3f|", s.direction_A_force.x, s.direction_A_force.y, ma);
    ImGui::Text("B (%.2f, %.2f)  |%.3f|", s.direction_B_force.x, s.direction_B_force.y, mb);
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, { 0.4f, 0.8f, 1.f, 1.f });
    ImGui::ProgressBar(std::clamp(ma * 0.01f, 0.f, 1.f), { -1.f, 4.f }, "");
    ImGui::ProgressBar(std::clamp(mb * 0.01f, 0.f, 1.f), { -1.f, 4.f }, "");
    ImGui::PopStyleColor();

    ImGui::Spacing(); ImGui::TextDisabled("Physical");
    ImGui::SetNextItemWidth(-1.f); ImGui::SliderFloat("##sk", &s.spring_const, 0.f, SpringGenome::max_spring_const, "K=%.3f");
    ImGui::SetNextItemWidth(-1.f); ImGui::SliderFloat("##sd", &s.damping, 0.f, SpringGenome::max_damping, "D=%.3f");
    ImGui::EndChild(); ImGui::SameLine();

    // ── Sinwave ──────────────────────────────────────────────────────────
    ImGui::BeginChild("SL_wave", { -1.f, -1.f }, true);
    ImGui::TextDisabled("Extension Sin Wave  rest-length ratio [0,1]");
    static float ext_buf[120];
    PlotUtils::fill_sinwave(ext_buf, s.amplitude, s.frequency, s.offset, s.vertical_shift, 0.f, 1.f);
    const int head = (int)(p->frames_alive % 120);
    PlotUtils::sinwave_graph("##sw", ext_buf, head, 0.f, 1.f, { -1.f, 52.f });
    ImGui::Text("t=%-3d  ratio = %.4f", head, ext_buf[head]);

    ImGui::Spacing();
    const float sw = (ImGui::GetContentRegionAvail().x - 6.f) * 0.5f;
    ImGui::SetNextItemWidth(sw);  ImGui::SliderFloat("##sA", &s.amplitude, 0.f, SpringGenome::max_amplitude, "A=%.3f");
    ImGui::SameLine(); ImGui::SetNextItemWidth(-1.f);
    ImGui::SliderFloat("##sB", &s.frequency, -SpringGenome::max_frequency, SpringGenome::max_frequency, "B=%.5f");
    ImGui::SetNextItemWidth(sw);  ImGui::SliderFloat("##sC", &s.offset, -SpringGenome::max_offset, SpringGenome::max_offset, "C=%.3f");
    ImGui::SameLine(); ImGui::SetNextItemWidth(-1.f);
    ImGui::SliderFloat("##sD", &s.vertical_shift, -SpringGenome::max_vertical_shift, SpringGenome::max_vertical_shift, "D=%.3f");
    ImGui::EndChild();
}

void OrganismTab::draw_tuning_tab(Protozoa* p)
{
    auto& cells = p->get_cells();
    auto& springs = p->get_springs();
    const float cw = (ImGui::GetContentRegionAvail().x - 18.f) / 3.f;

    // ── Mutation ─────────────────────────────────────────────────────────
    ImGui::BeginChild("TN_mut", { cw, -1.f }, true);
    ImGui::TextDisabled("Mutation");
    static float tun_rate = 0.2f, tun_range = 0.2f;
    ImGui::SetNextItemWidth(-1.f); ImGui::SliderFloat("##tr", &tun_rate, 0.f, 1.f, "rate  = %.3f");
    ImGui::SetNextItemWidth(-1.f); ImGui::SliderFloat("##trng", &tun_range, 0.f, 1.f, "range = %.3f");
    ImGui::Spacing();
    if (ImGui::Button("Apply Mutation", { -1.f, 0.f })) p->mutate(false, tun_rate, tun_range);
    ImGui::Spacing();
    ImGui::TextDisabled("Structure");
    if (ImGui::Button("Add Cell", { -1.f, 0.f })) p->mutate(true, 0.f, 0.f);
    ImGui::EndChild(); ImGui::SameLine();

    // ── Averages ─────────────────────────────────────────────────────────
    ImGui::BeginChild("TN_avg", { cw, -1.f }, true);
    ImGui::TextDisabled("Live Genome Averages");
    if (!cells.empty())
    {
        float sA = 0.f, sF = 0.f, sM = 0.f, sR = 0.f, sRad = 0.f;
        for (const Cell& c : cells) { sA += c.amplitude; sF += c.frequency; sM += c.mutation_rate; sR += c.mutation_range; sRad += c.radius; }
        const float n = (float)cells.size();
        ImGui::Text("Cells   %d", (int)n);
        ImGui::Text("Amp     %.4f", sA / n);
        ImGui::Text("Freq    %.6f", sF / n);
        ImGui::Text("Mut R   %.4f", sM / n);
        ImGui::Text("Rng     %.4f", sR / n);
        ImGui::Text("Radius  %.2f", sRad / n);
    }
    if (!springs.empty())
    {
        float sk = 0.f, sd = 0.f;
        for (const Spring& sp : springs) { sk += sp.spring_const; sd += sp.damping; }
        const float n = (float)springs.size();
        ImGui::Spacing();
        ImGui::Text("Springs %d", (int)n);
        ImGui::Text("Spr K   %.4f", sk / n);
        ImGui::Text("Spr D   %.4f", sd / n);
    }
    ImGui::EndChild(); ImGui::SameLine();

    // ── Danger zone ───────────────────────────────────────────────────────
    ImGui::BeginChild("TN_danger", { -1.f, -1.f }, true);
    ImGui::TextDisabled("Danger Zone");
    ImGui::Spacing();
    ImGui::PushStyleColor(ImGuiCol_Button, { 0.55f, 0.08f, 0.08f, 1.f });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.75f, 0.15f, 0.15f, 1.f });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 1.00f, 0.25f, 0.25f, 1.f });
    if (ImGui::Button("Kill Protozoa", { -1.f, 0.f })) p->kill();
    ImGui::PopStyleColor(3);
    ImGui::EndChild();
}

void OrganismTab::draw_controls_tab(UIContext& ctx, Protozoa* p)
{
    ImGui::SeparatorText("Lifecycle");

    static bool immortal = false;
    ImGui::Checkbox("Immortal (stub)##org", &immortal);
    ImGui::TextDisabled("  Needs World::set_immortal(id, bool)");

    ImGui::Spacing();
    if (ImGui::Button("Force Reproduce (stub)##org", { -1.f, 0.f }))
    { /* TODO: ctx.world.force_reproduce(p) */
    }

    ImGui::Spacing();
    if (ConfirmButton::draw("Force Die##org", { -1.f, 0.f }))
    {
        p->kill();
        ctx.world.deselect_protozoa();
    }

    ImGui::SeparatorText("Feed");
    static float feed_energy = 50.f;
    ImGui::SetNextItemWidth(-70.f);
    ImGui::SliderFloat("##feed", &feed_energy, 1.f, 500.f, "%.0f");
    ImGui::SameLine();
    if (ImGui::Button("Inject##org"))
    { /* TODO: p->add_energy(feed_energy) */
    }

    ImGui::SeparatorText("Clone & File");
    if (ImGui::Button("Clone organism nearby (stub)##org", { -1.f, 0.f }))
    { /* TODO: ctx.world.clone_protozoa(p) */
    }
    if (ImGui::Button("Save organism to file (stub)##org", { -1.f, 0.f }))
    { /* TODO: serialize p->genome to disk */
    }
    if (ImGui::Button("Load and spawn from file (stub)##org", { -1.f, 0.f }))
    { /* TODO: deserialize genome, spawn near p */
    }

    ImGui::SeparatorText("Tag");
    ImGui::TextDisabled("Use the Tagged tab to tag organism ID %d", p->id);
}