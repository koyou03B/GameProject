#pragma once
#include "HTN\PreconditionBase.h"

template<class TWorldState>
class AttackPrecondition : public PreconditionBase<TWorldState>
{
    bool CheckPreCondition(TWorldState& state) override
    {
        return state.canPrepareAttack == true;
    }
};
