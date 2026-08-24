#include "debug_tab.h"
#include <managers/cell_manager/cell_manager_settings.h>

void DebugTab::draw(const SimSnapshot& snap, ImGuiContext& ctx)
{
	const float total = ImGui::GetContentRegionAvail().x;
	const float sp = ImGui::GetStyle().ItemSpacing.x;
	const float cw = (total - sp * 2.f) / 3.f;
	const float ch = -1.f;

	// ── Playback ──────────────────────────────────────────────────────────────
	ImGui::BeginChild("Reproduction", { cw, ch }, true);

	ImGui::TextDisabled("Rendering");
	ImGui::Separator();

	if (ImGui::Checkbox("show only newborns", &ctx.cell_toggles.show_only_newborns))
		ctx.push({ .section = CommandSection::CellManagerEvent, .type = CommandType::SetCellToggles, .cell_toggles = ctx.cell_toggles });
	
	if (ImGui::Checkbox("show grid", &ctx.cell_toggles.show_newborn_grid))
		ctx.push({ .section = CommandSection::CellManagerEvent, .type = CommandType::SetCellToggles, .cell_toggles = ctx.cell_toggles });


	ImGui::EndChild();

	ImGui::BeginChild("Reproduction", { cw, ch }, true);

	ImGui::SetNextItemWidth(-1.f);
	float min_speed = snap.cell_manager_stats.min_speed; // sync with sim state
	if (ImGui::SliderFloat("##min_speed", &min_speed, 0.f, 2.f, "Min Speed %.3f"))
	{
		SimCommand cmd{ .section = CommandSection::CellManagerEvent, .type = CommandType::SetMinSpeed, .float_val = min_speed };
		ctx.push(cmd);
	}
	ImGui::Text("delta min speed (DMS): %.3f", CellManagerSettings::delta_min_speed);

	ImGui::EndChild();
}