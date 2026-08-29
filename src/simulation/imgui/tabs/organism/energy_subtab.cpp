#include "organism_tab.h"


// ─────────────────────────────────────────────────────────────────────────────
//  Energy tab
// ─────────────────────────────────────────────────────────────────────────────
void OrganismTab::draw_energy_tab(ImGuiContext& ctx, const SimSnapshot& snap)
{
    const OrganismTracker& tracker = snap.protozoa_tracker;
    const auto& cells = tracker.cells;
    const int              n = tracker.cell_count;

    if (n == 0) { ImGui::TextDisabled("No cells."); return; }

    // ── Aggregate totals not pre-computed by tracker ───────────────────────
    float total_integrity = 0.f;
    for (const Cell& c : cells)
        total_integrity += c.get_integrity();

    const float org_integrity_max = CellSettings::max_integrity * static_cast<float>(n);
    const float org_nutrients_max = CellSettings::max_nutrients * static_cast<float>(n);

    // ── Set up 3 columns with explicit widths ─────────────────────────────
    const float total_w = ImGui::GetContentRegionAvail().x;
    ImGui::Columns(3, "energy_cols", false);
    ImGui::SetColumnWidth(0, total_w * 0.45f);
    ImGui::SetColumnWidth(1, total_w * 0.25f);
    // column 2 takes the rest

    // ═════════════════════════════════════════════════════════════════════
    //  COLUMN 0 — Organism summary bars + per-cell grid
    // ═════════════════════════════════════════════════════════════════════
    ImGui::TextDisabled("Organism Summary  (%d cells)", n);
    ImGui::Separator();
    ImGui::Spacing();

    // Energy — green = at reproduce threshold, red = depleted
    {
        const float f = std::clamp(tracker.total_energy / tracker.max_energy, 0.f, 1.f);
        char lbl[48]; snprintf(lbl, sizeof(lbl), "%.1f / %.0f", tracker.total_energy, tracker.max_energy);
        labeled_bar("NRG", f, fraction_color(f), lbl, k_summary_bar_height);
    }
    // Nutrients — blue
    {
        const float f = std::clamp(tracker.total_nutrients / org_nutrients_max, 0.f, 1.f);
        char lbl[48]; snprintf(lbl, sizeof(lbl), "%.1f / %.0f", tracker.total_nutrients, org_nutrients_max);
        labeled_bar("NUT", f, { 0.2f, 0.6f, 1.0f, 1.f }, lbl, k_summary_bar_height);
    }
    // Integrity — yellow
    {
        const float f = std::clamp(total_integrity / org_integrity_max, 0.f, 1.f);
        char lbl[48]; snprintf(lbl, sizeof(lbl), "%.1f / %.0f", total_integrity, org_integrity_max);
        labeled_bar("INT", f, { 0.85f, 0.8f, 0.25f, 1.f }, lbl, k_summary_bar_height);
    }

    ImGui::Spacing();
    ImGui::Text("Spring drain this frame:  %.5f", tracker.spring_total_work_done);
    ImGui::Spacing();
    ImGui::Spacing();

    // ── Per-cell mini bar grid ─────────────────────────────────────────────
    ImGui::TextDisabled("Per-Cell  [ E = energy   N = nutrients   I = integrity ]");
    ImGui::Spacing();

    const float sp = ImGui::GetStyle().ItemSpacing.x;
    const float avail_w = ImGui::GetContentRegionAvail().x;
    const int   cells_per_row = std::max(1,
        static_cast<int>((avail_w + sp) / (k_mini_cell_box_width + sp)));

    for (int i = 0; i < n; ++i)
    {
        const Cell& c = cells[i];

        if (i > 0 && i % cells_per_row != 0)
            ImGui::SameLine();

        ImGui::PushID(i);
        ImGui::BeginGroup();

        ImGui::TextDisabled("C%d", i);
        ImGui::SameLine();
        ImGui::TextDisabled("(x%u)", c.total_food_eaten_);

        // Energy bar
        {
            const float f = std::clamp(c.get_energy() / CellSettings::max_energy, 0.f, 1.f);
            ImGui::TextDisabled("E"); ImGui::SameLine(0.f, 3.f);
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, fraction_color(f));
            ImGui::ProgressBar(f, { k_mini_cell_box_width, k_mini_bar_height }, "");
            ImGui::PopStyleColor();
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Energy: %.2f  /  %.0f (repro thresh)",
                    c.get_energy(), CellSettings::max_energy);
        }
        // Nutrients bar
        {
            const float f = std::clamp(c.nutrients_ / CellSettings::max_nutrients, 0.f, 1.f);
            ImGui::TextDisabled("N"); ImGui::SameLine(0.f, 3.f);
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4{ 0.2f, 0.6f, 1.0f, 1.f });
            ImGui::ProgressBar(f, { k_mini_cell_box_width, k_mini_bar_height }, "");
            ImGui::PopStyleColor();
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Nutrients: %.2f  /  %.0f (cap)", c.nutrients_, CellSettings::max_nutrients);
        }
        // Integrity bar
        {
            const float f = std::clamp(c.get_integrity() / CellSettings::max_integrity, 0.f, 1.f);
            ImGui::TextDisabled("I"); ImGui::SameLine(0.f, 3.f);
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4{ 0.85f, 0.8f, 0.25f, 1.f });
            ImGui::ProgressBar(f, { k_mini_cell_box_width, k_mini_bar_height }, "");
            ImGui::PopStyleColor();
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Integrity: %.2f  /  %.0f", c.get_integrity(), CellSettings::max_integrity);
        }

        ImGui::EndGroup();
        ImGui::PopID();
    }

    // ═════════════════════════════════════════════════════════════════════
    //  COLUMN 1 — Feed controls
    // ═════════════════════════════════════════════════════════════════════
    ImGui::NextColumn();

    ImGui::BeginChild("EN_feed", { -1.f, -1.f }, true);
    ImGui::TextDisabled("Feed");
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::RadioButton("Energy##fmode", &m_feed_mode_, 0);
    ImGui::SameLine();
    ImGui::RadioButton("Nutrients##fmode", &m_feed_mode_, 1);

    ImGui::Spacing();
    ImGui::SetNextItemWidth(-1.f);
    ImGui::SliderFloat("##feed_amount", &m_feed_amount_, 1.f, 500.f, "Amount = %.0f");

    ImGui::Spacing();
    if (ImGui::Button("Inject##en_inject", { -1.f, 0.f }))
    {
        SimCommand cmd{ .section = CommandSection::CellManagerEvent, .type = CommandType::InjectProtozoa };
        cmd.float_val = m_feed_amount_;
        cmd.bool_val = (m_feed_mode_ == 0);
        ctx.push(cmd);
    }
    ImGui::EndChild();

    // ═════════════════════════════════════════════════════════════════════
    //  COLUMN 2 — Conversion constants
    // ═════════════════════════════════════════════════════════════════════
    ImGui::NextColumn();

    ImGui::BeginChild("EN_const", { -1.f, -1.f }, true);
    ImGui::TextDisabled("Conversion rates  (per frame, per cell)");
    ImGui::Separator();
    ImGui::Text("Nutrients  ->  Energy    %.5f", CellSettings::nutrients_conversion_rate);
    ImGui::Text("Energy     ->  Integrity %.5f", CellSettings::integrity_conversion_rate);
    ImGui::Text("Spring work cost         %.5f", SpringSettings::spring_work_const);

    ImGui::Spacing();
    ImGui::TextDisabled("Thresholds  &  costs");
    ImGui::Separator();
    ImGui::Text("Initial energy           %.1f", CellSettings::initial_energy);
    ImGui::Text("Reproduce threshold      %.1f", CellSettings::max_energy);
    ImGui::Text("Integrity max            %.1f", CellSettings::max_integrity);
    ImGui::Text("Nutrients cap (*)        %.1f", CellSettings::max_nutrients);

    ImGui::Spacing();
    ImGui::TextDisabled("(*) placeholder — replace once ProtozoaSettings::max_nutrients exists");
    ImGui::EndChild();

    // ── Reset columns ──────────────────────────────────────────────────────
    ImGui::Columns(1);
}