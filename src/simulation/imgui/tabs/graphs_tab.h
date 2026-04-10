#pragma once
#include "i_tab.h"

class GraphsTab : public ITab
{
public:
    const char* label() const override { return "Graphs"; }
    void        draw(UIContext& ctx)   override;

private:
    float m_scroll_window_ = 60.f;
    bool  m_recording_ = false;
    float m_record_start_ = 0.f;
    bool  m_hover_paused_ = false;
    float m_paused_x_max_ = 0.f;

    // population series toggles
    bool m_show_protozoa_ = true;
    bool m_show_food_ = true;
    bool m_show_total_ = true;
    bool m_show_bands_ = true;

    // misc series toggles
    bool m_show_mut_rate_ = true;
    bool m_show_mut_range_ = true;
    bool m_show_offspring_ = true;
    bool m_show_lifetime_ = false;
    bool m_show_avg_cells_ = false;
    bool m_show_avg_springs_ = false;
    bool m_show_avg_energy_ = false;

    void draw_shared_toolbar(UIContext& ctx);
    void draw_population_tab(UIContext& ctx);
    void draw_event_markers(UIContext& ctx, float x_min, float x_max, float y_top);
    void draw_record_region(float x_max, float y_top);
    void draw_generations_tab(UIContext& ctx);
    void draw_misc_tab(UIContext& ctx);
};