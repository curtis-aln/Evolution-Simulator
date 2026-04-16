#pragma once
#include "../../sim_snapshot.h"

struct ITab
{
    virtual ~ITab() = default;
    virtual const char* label() const = 0;
    // snap is READ-ONLY display data
    // toggles is a mutable COPY you can write into freely
    virtual void draw(const SimSnapshot& snap, ImGuiContext& ctx) = 0;
};