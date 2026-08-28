#include "o_vector_tab.h"


// ═════════════════════════════════════════════════════════════════════════════
//  SUB-TAB 5 — Integrity
//  On-demand integrity checks with a scrollable pass/fail log.
// ═════════════════════════════════════════════════════════════════════════════

void OVecDebugTab::draw_integrity_tab(const SimSnapshot& snap, ImGuiContext& ctx)
{
    // ── Control row ───────────────────────────────────────────────────────────
    const bool run_all = ImGui::Button("Run All Checks");
    ImGui::SameLine();

    const bool run_cells = ImGui::Button("Cells##ic");   ImGui::SameLine();
    const bool run_food = ImGui::Button("Food##ic");    ImGui::SameLine();
    const bool run_bodies = ImGui::Button("Bodies##ic");  ImGui::SameLine();
    const bool run_springs = ImGui::Button("Springs##ic");

    ImGui::SameLine();
    if (ImGui::Button("Clear Log##ic"))
    {
        m_integrity_log_.clear();
        m_integrity_all_pass_ = true;
    }

    // ── Execute checks and capture output into m_integrity_log_ ───────────────
    //  integrity_check() writes to an ostream; we redirect it to a stringstream.
    auto run_check = [&](auto& dbg, const char* vec_name)
        {
            std::ostringstream oss;
            const bool ok = dbg.integrity_check(oss);
            m_integrity_all_pass_ = m_integrity_all_pass_ && ok;

            // Prefix each line with the vector name for clarity
            std::string line;
            std::istringstream iss(oss.str());
            while (std::getline(iss, line))
            {
                if (!line.empty())
                    m_integrity_log_.push_back(std::string("[") + vec_name + "]  " + line);
            }
        };

    if (run_all || run_cells)   run_check(m_dbg_cells_, "Cells");
    if (run_all || run_food)    run_check(m_dbg_food_, "Food");
    if (run_all || run_bodies)  run_check(m_dbg_bodies_, "Bodies");
    if (run_all || run_springs) run_check(m_dbg_springs_, "Springs");

    ImGui::Spacing();

    // ── Overall status banner ─────────────────────────────────────────────────
    if (m_integrity_log_.empty())
    {
        ImGui::TextDisabled("No checks run yet.  Press a button above.");
    }
    else if (m_integrity_all_pass_)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, k_col_ok);
        ImGui::Text("✓  All checks passed");
        ImGui::PopStyleColor();
    }
    else
    {
        ImGui::PushStyleColor(ImGuiCol_Text, k_col_fail);
        ImGui::Text("✗  One or more integrity failures detected!");
        ImGui::PopStyleColor();
    }

    // ── Lifetime failure counters ─────────────────────────────────────────────
    ImGui::Spacing();
    ImGui::TextDisabled("Lifetime integrity failures per vector:");
    ImGui::Columns(4, "##ic_fail_cols", false);

    const uint64_t failures[4] = {
        m_dbg_cells_.integrity_failures,
        m_dbg_food_.integrity_failures,
        m_dbg_bodies_.integrity_failures,
        m_dbg_springs_.integrity_failures,
    };

    for (int i = 0; i < 4; ++i)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, k_vec_colours[i]);
        if (failures[i] > 0)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, k_col_fail);
            ImGui::Text("%s  ✗ %llu", k_vec_names[i], failures[i]);
            ImGui::PopStyleColor();
        }
        else
        {
            ImGui::Text("%s  ✓ 0", k_vec_names[i]);
        }
        ImGui::PopStyleColor();
        if (i < 3) ImGui::NextColumn();
    }
    ImGui::Columns(1);

    // ── Scrollable log ────────────────────────────────────────────────────────
    section("Log");

    ImGui::BeginChild("##ic_log", { -1.f, -1.f }, true,
        ImGuiWindowFlags_HorizontalScrollbar);

    for (const std::string& entry : m_integrity_log_)
    {
        // Colour code: OK lines green, FAIL lines red, everything else default.
        const bool is_ok = entry.find("OK") != std::string::npos;
        const bool is_fail = entry.find("FAIL") != std::string::npos;

        if (is_ok)   ImGui::PushStyleColor(ImGuiCol_Text, k_col_ok);
        else if (is_fail) ImGui::PushStyleColor(ImGuiCol_Text, k_col_fail);

        ImGui::TextUnformatted(entry.c_str());

        if (is_ok || is_fail) ImGui::PopStyleColor();
    }

    // Auto-scroll to bottom when new log entries arrive
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);

    ImGui::EndChild();
}