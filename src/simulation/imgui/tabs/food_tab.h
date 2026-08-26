#pragma once
#include "i_tab.h"
#include "../.../../../../Utils/Graphics/pheromone_grid.h"

class FoodTab : public ITab
{
public:
    const char* label() const override { return "Food"; }
    void        draw(const SimSnapshot& snap, ImGuiContext& ctx)   override;

private:

};