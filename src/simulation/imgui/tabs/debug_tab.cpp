#include "debug_tab.h"

void DebugTab::draw(const SimSnapshot& snap, ImGuiContext& ctx)
{
	const float total = ImGui::GetContentRegionAvail().x;
	const float sp = ImGui::GetStyle().ItemSpacing.x;
	const float cw = (total - sp * 3.f) / 4.f;
	const float ch = -1.f;

	// ── Playback ──────────────────────────────────────────────────────────────
	ImGui::BeginChild("Reproduction", { cw, ch }, true);

	ImGui::TextDisabled("Rendering");
	ImGui::Separator();

	if (ImGui::Checkbox("show only newborns", &ctx.toggles.show_only_newborns))
		ctx.push({ .section = CommandSection::WorldEvent, .type = CommandType::SetWorldToggles, .toggles = ctx.toggles });
	if (ImGui::Checkbox("show grid", &ctx.toggles.show_only_newborns))
		ctx.push({ .section = CommandSection::WorldEvent, .type = CommandType::SetWorldToggles, .toggles = ctx.toggles });
	if (ImGui::Checkbox("show connect rad", &ctx.toggles.show_only_newborns))
		ctx.push({ .section = CommandSection::WorldEvent, .type = CommandType::SetWorldToggles, .toggles = ctx.toggles });


	ImGui::EndChild();
}