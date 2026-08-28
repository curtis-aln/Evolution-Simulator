#include "o_vector_tab.h"


// ═════════════════════════════════════════════════════════════════════════════
//  SUB-TAB 4 — Snapshots
//  Take, label, list, and diff named snapshots of any vector.
// ═════════════════════════════════════════════════════════════════════════════

void OVecDebugTab::draw_snapshots_tab(const SimSnapshot& snap, ImGuiContext& ctx)
{
    // ── Vector selector ───────────────────────────────────────────────────────
    ImGui::TextDisabled("Vector:");
    ImGui::SameLine();
    for (int i = 0; i < 4; ++i)
    {
        if (i > 0) ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, k_vec_colours[i]);
        if (ImGui::RadioButton(k_vec_names[i], m_snap_vec_sel_ == i))
        {
            m_snap_vec_sel_ = i;
            m_snap_a_idx_ = 0;
            m_snap_b_idx_ = 0;
        }
        ImGui::PopStyleColor();
    }

    ImGui::Separator();
    ImGui::Spacing();

    // ── Take snapshot button + optional label ─────────────────────────────────
    static char snap_label_buf[64] = "";
    ImGui::SetNextItemWidth(200.f);
    ImGui::InputText("Label##snap_lbl", snap_label_buf, sizeof(snap_label_buf));
    ImGui::SameLine();

    if (ImGui::Button("Take Snapshot"))
    {
        // IMGUI TODO: dispatch to the correct debug instance via m_snap_vec_sel_.
        // Example:
        //   const std::string lbl = strlen(snap_label_buf) > 0
        //       ? snap_label_buf : "";
        //   switch (m_snap_vec_sel_) {
        //       case 0: m_dbg_cells_.take_snapshot(lbl);   break;
        //       case 1: m_dbg_food_.take_snapshot(lbl);    break;
        //       case 2: m_dbg_bodies_.take_snapshot(lbl);  break;
        //       case 3: m_dbg_springs_.take_snapshot(lbl); break;
        //   }
        //   snap_label_buf[0] = '\0'; // clear label
        ImGui::TextDisabled("(snapshot would be taken here)"); // stub
    }

    ImGui::Spacing();

    // ── Snapshot table for the selected vector ────────────────────────────────
    section("Snapshot History");

    // IMGUI TODO: retrieve snapshot list from the active debug instance.
    //  const int n_snaps = (m_snap_vec_sel_ == 0) ? m_dbg_cells_.snapshot_count() : ...
    //  Use dbg.get_snapshot(i) to read each Snapshot struct.

    const int n_snaps_stub = 0; // stub
    if (n_snaps_stub == 0)
    {
        ImGui::TextDisabled("No snapshots taken yet.");
    }
    else if (ImGui::BeginTable("##snap_table", 6,
        ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
        ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingFixedFit,
        { -1.f, 140.f }))
    {
        ImGui::TableSetupColumn("#", ImGuiTableColumnFlags_WidthFixed, 30.f);
        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Active", ImGuiTableColumnFlags_WidthFixed, 60.f);
        ImGui::TableSetupColumn("Free", ImGuiTableColumnFlags_WidthFixed, 50.f);
        ImGui::TableSetupColumn("Fill", ImGuiTableColumnFlags_WidthFixed, 55.f);
        ImGui::TableSetupColumn("Frag", ImGuiTableColumnFlags_WidthFixed, 55.f);
        ImGui::TableHeadersRow();

        // IMGUI TODO: iterate real snapshots.
        // for (int i = 0; i < n_snaps; ++i) {
        //     const auto& sn = active_dbg.get_snapshot(i);
        //     ImGui::TableNextRow();
        //     ImGui::TableSetColumnIndex(0); ImGui::Text("%d",   i);
        //     ImGui::TableSetColumnIndex(1); ImGui::TextUnformatted(sn.label.c_str());
        //     ImGui::TableSetColumnIndex(2); ImGui::Text("%d",   sn.active_count);
        //     ImGui::TableSetColumnIndex(3); ImGui::Text("%d",   sn.free_count);
        //     ImGui::TableSetColumnIndex(4); ImGui::Text("%.2f", sn.fill_ratio);
        //     ImGui::TableSetColumnIndex(5); ImGui::Text("%.2f", sn.fragmentation);
        // }
        ImGui::EndTable();
    }

    // ── Diff panel ────────────────────────────────────────────────────────────
    section("Diff Two Snapshots");

    ImGui::SetNextItemWidth(120.f);
    ImGui::InputInt("From (index)##snap_a", &m_snap_a_idx_);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(120.f);
    ImGui::InputInt("To (index)##snap_b", &m_snap_b_idx_);
    ImGui::SameLine();

    if (ImGui::Button("Diff##run_diff") && n_snaps_stub >= 2)
    {
        // IMGUI TODO: call active_dbg.diff_snapshots(m_snap_a_idx_, m_snap_b_idx_)
        // and display the SnapshotDiff struct fields below.
    }

    // IMGUI TODO: show the last computed SnapshotDiff in a table here.
    // Fields: delta_active, delta_free, delta_array_size, delta_fill_ratio,
    //         delta_fragmentation, delta_emplaces, delta_adds, delta_removes,
    //         delta_failed_adds, delta_churn, elapsed_ms.
    ImGui::TextDisabled("[Diff results will appear here after clicking Diff]");
}
