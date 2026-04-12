#pragma once
#include "../../shared_state.h"

struct ITab
{
    virtual ~ITab() = default;
    virtual const char* label() const = 0;
    virtual void        draw(SimSnapshot& snapshot) = 0;
};