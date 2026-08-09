#include "food_tab.h"

void FoodTab::draw(const SimSnapshot& snap, ImGuiContext& ctx)
{
	const float total = ImGui::GetContentRegionAvail().x;
	const float sp = ImGui::GetStyle().ItemSpacing.x;
	const float cw = (total - sp * 3.f) / 4.f;
	const float ch = -1.f;

	// ── Playback ──────────────────────────────────────────────────────────────
	ImGui::BeginChild("Food", { cw, ch }, true);

	ImGui::TextDisabled("Food");
	ImGui::Separator();

	ImGui::EndChild();
}