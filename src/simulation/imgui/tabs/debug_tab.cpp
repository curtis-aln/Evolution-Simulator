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
	if (ImGui::SliderFloat("##min_speed", &min_speed, 0.f, 10.f, "Min Speed %.3f"))
	{
		SimCommand cmd{ .section = CommandSection::CellManagerEvent, .type = CommandType::SetMinSpeed, .float_val = min_speed };
		ctx.push(cmd);
	}
	ImGui::Text("delta min speed (DMS): %.3f", CellManagerSettings::delta_min_speed);
	ImGui::Spacing();
	ImGui::Separator();
	toggle(snap, ctx, "Impulse Damage", &CellManagerToggles::collision_integrity_damage_);
	if (ctx.cell_toggles.collision_integrity_damage_)
	{
		ImGui::Indent();
		float thresh = CellManagerSettings::impulse_damage_thresh;
		if (ImGui::SliderFloat("##impulse_damage_thresh", &thresh, 0.f, 100.f, "threshold %.2f"))
			ctx.push({ .section = CommandSection::CellManagerEvent, .type = CommandType::SetImpulseDamageThreshold, .float_val = thresh });
		float damage = CellManagerSettings::impulse_damage_multiplier;
		if (ImGui::SliderFloat("##impulse_damage_multiplier", &damage, 0.f, 0.002f, "multiplier %.5f"))
			ctx.push({ .section = CommandSection::CellManagerEvent, .type = CommandType::SetImpulseDamageMultiplier, .float_val = damage });
		float max = CellManagerSettings::max_single_hit_integrity_fraction;
		if (ImGui::SliderFloat("##max_single_hit_integrity_fraction", &max, 0.f, 1.f, "max single hit integrity fraction %.2f"))
			ctx.push({ .section = CommandSection::CellManagerEvent, .type = CommandType::SetMaxImpulseDamage, .float_val = max });
		ImGui::Unindent();
	}

	toggle(snap, ctx, "spring stress integrity damage", &CellManagerToggles::spring_stress_integrity_damage);
	toggle(snap, ctx, "spring too long breakage", &CellManagerToggles::spring_too_long_breakage);
	toggle(snap, ctx, "spring too much force breakage", &CellManagerToggles::spring_too_much_force_breakage);

	ImGui::Spacing();
	ImGui::TextDisabled("Energy Tax");
	toggle(snap, ctx, "work done energy", &CellManagerToggles::work_done_energy);

	toggle(snap, ctx, "friction energy loss", &CellManagerToggles::friction_energy_loss);

	// Sets the Lifetime of cell matter
	int lifetime = CellMatterSettings::max_time_to_live;
	if (ImGui::SliderInt("##cell_matter_lifetime", &lifetime, 0, 30000, "cell matter lifetime %d"))
		ctx.push({ .section = CommandSection::CellManagerEvent, .type = CommandType::SetCellMatterLifetime, .int_val = lifetime });

	// Setting the amount of Sub-iterations the program does
	int sub_iterations = WorldSettings::tick_sim_multiplier;
	if (ImGui::SliderInt("##sub_iterations", &sub_iterations, 1, 100, "update sub-iterations %d"))
		ctx.push({ .section = CommandSection::WorldEvent, .type = CommandType::SetSubIterations, .int_val = sub_iterations });

	ImGui::EndChild();
}