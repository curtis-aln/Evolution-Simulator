#pragma once
#include "i_tab.h"
#include "../../../Protozoa/Protozoa.h"

class OrganismTab : public ITab
{
public:
    const char* label() const override { return "Organism"; }
    void        draw(const SimSnapshot& snap, ImGuiContext& ctx)   override;

private:
    int  m_last_id_ = -1;
    int  m_sel_cell_idx_ = 0;
    int  m_sel_spring_idx_ = 0;
    bool m_sel_is_spring_ = false;
    int  m_wave_cycles_ = 1;   // number of full periods shown in the sinwave graph

    void draw_no_selection();
    void draw_overview(const Protozoa& p);
    void draw_cells_springs_tab(const Protozoa& p);
    void draw_cell_detail(const Protozoa&, const Cell& c);
    void draw_spring_detail(const Protozoa&, const Spring& s);
    void draw_tuning_controls_tab(const SimSnapshot& snap);
};