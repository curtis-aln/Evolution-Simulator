#pragma once
#include "i_tab.h"

class SimulationTab : public ITab
{
public:
    const char* label() const override { return "Simulation"; }
    void        draw(UIContext& ctx)   override;

private:
    enum class FFCondition { Duration, Population, Generation };
    FFCondition m_ff_cond_ = FFCondition::Duration;
    float       m_ff_target_ = 300.f;
    bool        m_fast_fwd_ = false;
    float       m_speed_ = 1.f;

    void draw_playback(UIContext& ctx);
    void draw_fast_forward();
    void draw_world_settings(UIContext& ctx);
    void draw_io_stubs();
    void draw_keybind_table();
};