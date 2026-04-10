#include "statistics_tab.h"
#include "../helpers/stat_row.h"
#include "../helpers/plot_utils.h"
#include <imgui.h>

void StatisticsTab::draw(UIContext& ctx)
{
    // ── 5 equal-width panels side by side ─────────────────────────────────────
    const float total = ImGui::GetContentRegionAvail().x;
    const float sp = ImGui::GetStyle().ItemSpacing.x;
    const float cw = (total - sp * 4.f) / 5.f;
    const float ch = -ImGui::GetFrameHeightWithSpacing() - 4.f; // leave room for bottom button

    // ── Performance ───────────────────────────────────────────────────────────
    ImGui::BeginChild("ST_perf", { cw, ch }, true);
    ImGui::TextDisabled("Performance");
    ImGui::Separator();
    StatRow::draw("FPS", "%.1f", ctx.fps);
    StatRow::draw("Frame", "%u", ctx.ticks);
    StatRow::draw("Elapsed", "%s", PlotUtils::format_time(ctx.total_time_elapsed).c_str());
    ImGui::EndChild();
    ImGui::SameLine();

    // ── Population ────────────────────────────────────────────────────────────
    ImGui::BeginChild("ST_pop", { cw, ch }, true);
    ImGui::TextDisabled("Population");
    ImGui::Separator();
    const int  p = ctx.world.get_protozoa_count();
    const int  f = ctx.world.get_food_count();
    const bool risk = p <= 10;
    StatRow::draw("Protozoa", "%d", p);
    StatRow::draw("Food", "%d", f);
    StatRow::draw("Total", "%d", p + f);
    StatRow::draw_warn("Ext. risk", risk, "%s", risk ? "YES" : "no");
    StatRow::draw("Peak ever", "%d", ctx.world.peak_protozoa_ever_);
    ImGui::EndChild();
    ImGui::SameLine();

    // ── Vitals ────────────────────────────────────────────────────────────────
    ImGui::BeginChild("ST_vit", { cw, ch }, true);
    ImGui::TextDisabled("Vitals");
    ImGui::Separator();
    StatRow::draw("Avg lifetime", "%.1f fr", ctx.world.average_lifetime_);
    StatRow::draw("Longest ever", "%d fr", ctx.world.longest_lived_ever_);
    StatRow::draw("Births /100f", "%.1f", ctx.world.births_per_hundered_frames_);
    StatRow::draw("Deaths /100f", "%.1f", ctx.world.deaths_per_hundered_frames_);
    StatRow::draw("Infant mortality", "%.1f%%", ctx.world.infant_mortality_rate_ * 100.f);
    ImGui::EndChild();
    ImGui::SameLine();

    // ── Genetics ──────────────────────────────────────────────────────────────
    ImGui::BeginChild("ST_gen", { cw, ch }, true);
    ImGui::TextDisabled("Genetics");
    ImGui::Separator();
    StatRow::draw("Avg generation", "%.2f", ctx.world.average_generation_);
    StatRow::draw("Highest gen ever", "%d", ctx.world.highest_generation_ever_);
    StatRow::draw("Most offspring", "%d", ctx.world.most_offspring_ever_);
    StatRow::draw("Frames / gen", "%.0f", ctx.world.frames_per_generation_);
    StatRow::draw("Avg mut rate", "%.4f", ctx.world.average_mutation_rate_);
    StatRow::draw("Avg mut range", "%.4f", ctx.world.average_mutation_range_);
    StatRow::draw("Diversity", "%.4f", ctx.world.genetic_diversity_);
    ImGui::EndChild();
    ImGui::SameLine();

    // ── Morphology ────────────────────────────────────────────────────────────
    ImGui::BeginChild("ST_morph", { -1.f, ch }, true);
    ImGui::TextDisabled("Morphology");
    ImGui::Separator();
    StatRow::draw("Avg cells", "%.2f", ctx.world.average_cells_per_protozoa_);
    StatRow::draw("Avg springs", "%.2f", ctx.world.average_spring_count_);
    StatRow::draw("Avg offspring", "%.2f", ctx.world.average_offspring_count_);
    StatRow::draw("Avg energy", "%.1f", ctx.world.average_energy_);
    StatRow::draw("Energy ratio", "%.3f", ctx.world.energy_efficiency_);
    ImGui::EndChild();

    // ── Bottom button (full width) ─────────────────────────────────────────────
    ImGui::Spacing();
    if (ImGui::Button("Navigate to most successful organism", { -1.f, 0.f }))
    { /* TODO: World::get_most_successful() + camera lock */
    }
}