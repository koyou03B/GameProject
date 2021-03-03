#pragma once
#include "HTN\PreconditionBase.h"

template<class TWorldState>
class HealPrecondition : public PreconditionBase<TWorldState>
{
    bool CheckPreCondition(TWorldState& state) override
    {
        return state._canRecover == true;
    }
};
