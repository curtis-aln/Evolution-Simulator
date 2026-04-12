#pragma once
#include "i_tab.h"

class SimulationTab : public ITab
{
public:
    const char* label() const override { return "Simulation"; }
    void        draw(SimSnapshot& snapshot)   override;

private:
    enum class FFCondition { Duration, Population, Generation };
    FFCondition m_ff_cond_ = FFCondition::Duration;
    float       m_ff_target_ = 300.f;
    bool        m_fast_fwd_ = false;
    float       m_speed_ = 1.f;
};