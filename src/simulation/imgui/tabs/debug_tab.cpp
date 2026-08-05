#include "debug_tab.h"

void DebugTab::draw(const SimSnapshot& snap, ImGuiContext& ctx)
{
	const float total = ImGui::GetContentRegionAvail().x;
	const float sp = ImGui::GetStyle().ItemSpacing.x;
	const float cw = (total - sp * 3.f) / 4.f;
	const float ch = -1.f;

	// ── Playback ──────────────────────────────────────────────────────────────
	ImGui::BeginChild("Test", { cw, ch }, true);
	ImGui::TextDisabled("Playback");
	ImGui::Separator();
	ImGui::EndChild();
}