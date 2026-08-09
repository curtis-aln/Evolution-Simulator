#pragma once
#include "i_tab.h"

class FoodTab : public ITab
{
public:
    const char* label() const override { return "Food"; }
    void        draw(const SimSnapshot& snap, ImGuiContext& ctx)   override;

private:

};