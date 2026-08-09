#include "food_tab.h"

void FoodTab::draw(const SimSnapshot& snap, ImGuiContext& ctx)
{
	const float total = ImGui::GetContentRegionAvail().x;
	const float sp = ImGui::GetStyle().ItemSpacing.x;
	const float cw = (total - sp * 2.f) / 3.f;
	const float ch = -1.f;

	ImGui::BeginChild("Food", { cw, ch }, true);

	ImGui::TextDisabled("Food");
	ImGui::Separator();

	if (ImGui::Checkbox("spawn random food", &ctx.food_toggles.spawn_random_food))
		ctx.push({ .section = CommandSection::FoodManagerEvent, .type = CommandType::SetFoodToggles, .food_toggles = ctx.food_toggles });

	if (ctx.food_toggles.spawn_random_food)
	{
		ImGui::Indent();
		int intensity = snap.food_manager_stats.food_random_spawn_intensity;
		if (ImGui::SliderInt("##random_intensity", &intensity, 0, FoodManagerSettings::max_random_food_spawned_per_frame, "intensity %d"))
			ctx.push({ .section = CommandSection::FoodManagerEvent, .type = CommandType::SetRandomIntensity, .int_val = intensity });
		ImGui::Unindent();
	}

	ImGui::Separator();

	if (ImGui::Checkbox("food mitosis", &ctx.food_toggles.food_mitosis))
		ctx.push({ .section = CommandSection::FoodManagerEvent, .type = CommandType::SetFoodToggles, .food_toggles = ctx.food_toggles });

	if (ctx.food_toggles.food_mitosis)
	{
		ImGui::Indent();
		float intensity = snap.food_manager_stats.food_mitosis_spawn_intensity;
		if (ImGui::SliderFloat("##mitosis_intensity", &intensity, 0.f, 4.f, "intensity %.2f"))
			ctx.push({ .section = CommandSection::FoodManagerEvent, .type = CommandType::SetMitosisIntensity, .float_val = intensity });
		ImGui::Unindent();
	}

	ImGui::EndChild();
}