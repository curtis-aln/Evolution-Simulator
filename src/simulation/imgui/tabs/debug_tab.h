#pragma once

#include "i_tab.h"


class DebugTab : public ITab
{
public:
    const char* label() const override { return "Debug"; }
    void        draw(const SimSnapshot& snap, ImGuiContext& ctx)   override;

};