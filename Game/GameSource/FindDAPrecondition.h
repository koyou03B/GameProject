#pragma once
#include "HTN\PreconditionBase.h"

template<class TWorldState>
class FindDAPrecondition : public PreconditionBase<TWorldState>
{
    bool CheckPreCondition(TWorldState& state) override
    {
        return state._canFindDirectionToAvoid == false && state._canAvoid == false;
    }
};
