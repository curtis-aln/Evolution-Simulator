#pragma once
#include "../ui_context.h"

struct ITab
{
    virtual ~ITab() = default;
    virtual const char* label() const = 0;
    virtual void        draw(UIContext& ctx) = 0;
};