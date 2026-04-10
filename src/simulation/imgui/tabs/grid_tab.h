#pragma once
#include "i_tab.h"
#include "../../../Utils/Graphics/spatial_grid/simple_spatial_grid.h"

class GridTab : public ITab
{
public:
    const char* label() const override { return "Grid"; }
    void        draw(UIContext& ctx)   override;

private:
    void draw_grid_info(const char* label, SimpleSpatialGrid& grid, bool tracking);
    void draw_tuning(UIContext& ctx);
};