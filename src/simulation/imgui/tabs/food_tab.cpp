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
	ImGui::SameLine(); 

	// ----------------------------------------------

	ImGui::BeginChild("Pheromones", { cw, ch }, true);

	ImGui::TextDisabled("Pheromones");
	ImGui::Separator();
	ImGui::Spacing();

	// two toggles
	toggle(snap, ctx, "update pheromone grid", &FoodToggles::update_pheromone_grid);
	toggle(snap, ctx, "render pheromone grid", &FoodToggles::render_pheromone_grid);

	// int slider for updating steps 1 - 5
	int updating_steps = PheromoneGridSettings::substeps;
	if (ImGui::SliderInt("##updating_steps", &updating_steps, 1, 5, "Updating Steps %d"))
		ctx.push({ .section = CommandSection::FoodManagerEvent, .type = CommandType::SetPheromoneUpdatingSteps, .int_val = updating_steps });

	// float sliders for decay, diffusion
	float decay_rate = PheromoneGridSettings::decay_rate;
	if (ImGui::SliderFloat("##decay_rate", &decay_rate, 0.0001f, 0.01f, "Decay Rate %.4f"))
		ctx.push({ .section = CommandSection::FoodManagerEvent, .type = CommandType::SetPheromoneDecayRate, .float_val = decay_rate });

	float diffuse_rate = PheromoneGridSettings::diffuse_rate;
	if (ImGui::SliderFloat("##diffuse_rate", &diffuse_rate, 0.f, 1.f, "Diffuse Rate %.2f"))
		ctx.push({ .section = CommandSection::FoodManagerEvent, .type = CommandType::SetPheromoneDiffusionRate, .float_val = diffuse_rate });

	// float slider for deposit amount and max pheromone level
	float deposit_amount = PheromoneGridSettings::deposit_amount;
	if (ImGui::SliderFloat("##deposit_amount", &deposit_amount, 0.f, 50.f, "Deposit Amount %.2f"))
		ctx.push({ .section = CommandSection::FoodManagerEvent, .type = CommandType::SetPheromoneDepositAmount, .float_val = deposit_amount });

	float max_pheromone = PheromoneGridSettings::max_pheromone;
	if (ImGui::SliderFloat("##max_pheromone", &max_pheromone, 20.f, 200.f, "Max Pheromone %.2f"))
		ctx.push({ .section = CommandSection::FoodManagerEvent, .type = CommandType::SetPheromoneMaxPheromone, .float_val = max_pheromone });
	
	ImGui::EndChild();
}