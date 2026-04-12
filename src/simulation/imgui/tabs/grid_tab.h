#pragma once
#include "i_tab.h"
#include "../../../Utils/Graphics/spatial_grid/simple_spatial_grid.h"

class GridTab : public ITab
{
public:
    const char* label() const override { return "Grid"; }
    void        draw(SimSnapshot& snapshot)   override;

private:
    //void draw_tuning(SimSnapshot& snapshot);
    void draw_grid_info(const char* label, SpatialGridData& grid, bool tracking);
};