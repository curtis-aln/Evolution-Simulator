#pragma once
#include "i_tab.h"

class StatisticsTab : public ITab
{
public:
    const char* label() const override { return "Statistics"; }
    void        draw(SimSnapshot& snapshot)   override;
};