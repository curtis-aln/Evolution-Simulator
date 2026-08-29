#pragma once

#include "i_tab.h"
#include "../../../entities/matter/matter_settings.h"
#include "../../../world/world_settings.h"


class DebugTab : public ITab
{
public:
    const char* label() const override { return "Debug"; }
    void        draw(const SimSnapshot& snap, ImGuiContext& ctx)   override;

};