#pragma once
#include "i_tab.h"

class DisplayTab : public ITab
{
public:
    const char* label() const override { return "Display"; }
    void        draw(UIContext& ctx)   override;

private:
    int   m_camera_lock_ = 0;
    float m_zoom_slider_ = 1.f;
    float m_ui_scale_ = 100.f;
};