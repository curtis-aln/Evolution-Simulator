#include "control_panel.h"
#include "tabs/simulation_tab.h"
#include "tabs/graphs/graphs_tab.h"
#include "tabs/organism/organism_tab.h"
#include "tabs/grid_tab.h"
#include "tabs/o_vector/o_vector_tab.h"
#include "tabs/debug_tab.h"
#include "tabs/food_tab.h"
#include <imgui.h>
#include <cstring>

#include "imgui_settings.h"

ControlPanel::ControlPanel()
{
    m_tabs_.push_back(std::make_unique<SimulationTab>());
    m_tabs_.push_back(std::make_unique<GraphsTab>());
    m_tabs_.push_back(std::make_unique<OrganismTab>());
    m_tabs_.push_back(std::make_unique<GridTab>());
    m_tabs_.push_back(std::make_unique<OVecDebugTab>());
    m_tabs_.push_back(std::make_unique<DebugTab>());
    m_tabs_.push_back(std::make_unique<FoodTab>());
}

void ControlPanel::select_tab(const char* label)
{
    m_pending_tab_label_ = label;
}

void ControlPanel::draw(const SimSnapshot& snap, ImGuiContext& ctx, float dt)
{
    ImGui::SetNextWindowPos({ 10.f, 10.f }, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize({ 520.f, 640.f }, ImGuiCond_FirstUseEver);

    ImGui::Begin(control_panel_window_name, nullptr, ImGuiWindowFlags_NoNav);

    if (ImGui::BeginTabBar("##ctrl_tabs"))
    {
        for (auto& tab : m_tabs_)
        {
            ImGuiTabItemFlags flags = 0;
            if (m_pending_tab_label_ && std::strcmp(tab->label(), m_pending_tab_label_) == 0)
                flags |= ImGuiTabItemFlags_SetSelected;

            if (ImGui::BeginTabItem(tab->label(), nullptr, flags))
            {
                tab->draw(snap, ctx);
                ImGui::EndTabItem();
            }
        }
        ImGui::EndTabBar();
    }

    m_pending_tab_label_ = nullptr; // one-shot: consume the request

    ImGui::End();
}