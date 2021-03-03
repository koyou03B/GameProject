#pragma once
#include "HTN\PreconditionBase.h"

template<class TWorldState>
class AvoidPrecondition : public PreconditionBase<TWorldState>
{
    bool CheckPreCondition(TWorldState& state) override
    {
        return state._canFindDirectionToAvoid == true;
    }
};
