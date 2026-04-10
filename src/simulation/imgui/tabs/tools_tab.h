#pragma once
#include "i_tab.h"

class ToolsTab : public ITab
{
public:
    const char* label() const override { return "Tools"; }
    void        draw(UIContext& ctx)   override;

private:
    bool  m_blackhole_ = false;
    float m_bh_strength_ = 500.f;
    float m_bh_radius_ = 1000.f;
    int   m_spawn_n_ = 10;
    bool  m_spawn_mut_ = true;
    float m_mut_rate_ = 0.3f;
    float m_mut_range_ = 0.3f;

    void draw_blackhole();
    void draw_spawn(UIContext& ctx);
    void draw_clear(UIContext& ctx);
};