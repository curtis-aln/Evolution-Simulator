#pragma once
#include "i_tab.h"

class StatisticsTab : public ITab
{
public:
    const char* label() const override { return "Statistics"; }
    void        draw(UIContext& ctx)   override;

private:
    void draw_performance(UIContext& ctx);
    void draw_population(UIContext& ctx);
    void draw_vitals(UIContext& ctx);
    void draw_genetics(UIContext& ctx);
    void draw_morphology(UIContext& ctx);
};