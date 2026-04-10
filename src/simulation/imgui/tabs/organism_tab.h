#pragma once
#include "i_tab.h"
#include "../../../Protozoa/Protozoa.h"
#include "../../../Protozoa/genetics/CellGenome.h"
#include "../../../Protozoa/genetics/SpringGenome.h"

class OrganismTab : public ITab
{
public:
    const char* label() const override { return "Organism"; }
    void        draw(UIContext& ctx)   override;

private:
    int m_last_id_ = -1;
    int m_sel_cell_idx_ = 0;
    int m_sel_spring_idx_ = 0;

    void draw_no_selection();
    void draw_overview(Protozoa* p);

    // new member
    bool m_sel_is_spring_ = false;

    // new private methods (replacing the old draw_cells_tab / draw_springs_tab / draw_tuning_tab / draw_controls_tab)
    void draw_cell_detail(Protozoa* p, Cell& c);
    void draw_spring_detail(Protozoa* p, Spring& s);
    void draw_cells_springs_tab(Protozoa* p);
    void draw_tuning_controls_tab(UIContext& ctx, Protozoa* p);
};