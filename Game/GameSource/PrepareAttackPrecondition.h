#pragma once
#include "HTN\PreconditionBase.h"

template<class TWorldState>
class PrepareAttackPrecondition : public PreconditionBase<TWorldState>
{
    bool CheckPreCondition(TWorldState& state) override
    {
        return state.canPrepareAttack == false &&
            state.canAttack == false;
    }
};
