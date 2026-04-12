#include "statistics_tab.h"
#include "../helpers/stat_row.h"
#include "../helpers/plot_utils.h"
#include <imgui.h>

void StatisticsTab::draw(SimSnapshot& snapshot)
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
    StatRow::draw("FPS", "%.1f", snapshot.stats.fps);
    StatRow::draw("Frame", "%u", snapshot.iterations_);
    StatRow::draw("Elapsed", "%s", PlotUtils::format_time(snapshot.total_time_elapsed).c_str());
    ImGui::EndChild();
    ImGui::SameLine();

    // ── Population ────────────────────────────────────────────────────────────
    ImGui::BeginChild("ST_pop", { cw, ch }, true);
    ImGui::TextDisabled("Population");
    ImGui::Separator();
	const int  p = snapshot.stats.protozoa_count;
    const int  f = snapshot.stats.food_count;
    const bool risk = p <= 10;
    StatRow::draw("Protozoa", "%d", p);
    StatRow::draw("Food", "%d", f);
    StatRow::draw("Total", "%d", p + f);
    StatRow::draw_warn("Ext. risk", risk, "%s", risk ? "YES" : "no");
    StatRow::draw("Peak ever", "%d", snapshot.stats.peak_protozoa_ever);
    ImGui::EndChild();
    ImGui::SameLine();

    // ── Vitals ────────────────────────────────────────────────────────────────
    ImGui::BeginChild("ST_vit", { cw, ch }, true);
    ImGui::TextDisabled("Vitals");
    ImGui::Separator();
    StatRow::draw("Avg lifetime", "%.1f fr", snapshot.stats.average_lifetime);
    StatRow::draw("Longest ever", "%d fr", snapshot.stats.longest_lived_ever);
    StatRow::draw("Births /100f", "%.1f", snapshot.stats.births_per_hundered_frames);
    StatRow::draw("Deaths /100f", "%.1f", snapshot.stats.deaths_per_hundered_frames);
    StatRow::draw("Infant mortality", "%.1f%%", snapshot.stats.infant_mortality_rate * 100.f);
    ImGui::EndChild();
    ImGui::SameLine();

    // ── Genetics ──────────────────────────────────────────────────────────────
    ImGui::BeginChild("ST_gen", { cw, ch }, true);
    ImGui::TextDisabled("Genetics");
    ImGui::Separator();
    StatRow::draw("Avg generation", "%.2f", snapshot.stats.average_generation);
    StatRow::draw("Highest gen ever", "%d", snapshot.stats.highest_generation_ever);
    StatRow::draw("Most offspring", "%d", snapshot.stats.most_offspring_ever);
    StatRow::draw("Frames / gen", "%.0f", snapshot.stats.frames_per_generation);
    StatRow::draw("Avg mut rate", "%.4f", snapshot.stats.average_mutation_rate);
    StatRow::draw("Avg mut range", "%.4f", snapshot.stats.average_mutation_range);
    StatRow::draw("Diversity", "%.4f", snapshot.stats.genetic_diversity);
    ImGui::EndChild();
    ImGui::SameLine();

    // ── Morphology ────────────────────────────────────────────────────────────
    ImGui::BeginChild("ST_morph", { -1.f, ch }, true);
    ImGui::TextDisabled("Morphology");
    ImGui::Separator();
    StatRow::draw("Avg cells", "%.2f", snapshot.stats.average_cells_per_protozoa);
    StatRow::draw("Avg springs", "%.2f", snapshot.stats.average_spring_count);
    StatRow::draw("Avg offspring", "%.2f", snapshot.stats.average_offspring_count);
    StatRow::draw("Avg energy", "%.1f", snapshot.stats.average_energy);
    StatRow::draw("Energy ratio", "%.3f", snapshot.stats.energy_efficiency);
    ImGui::EndChild();

    // ── Bottom button (full width) ─────────────────────────────────────────────
    ImGui::Spacing();
    if (ImGui::Button("Navigate to most successful organism", { -1.f, 0.f }))
    { /* TODO: World::get_most_successful() + camera lock */
    }
}