#pragma once
#include "HTN\PrimitiveTask.h"

template<class TWorldState, class TChara>
class SetArrowTask :public PrimitiveTask<TWorldState, TChara>
{
public:
    SetArrowTask()
    {
        this->m_taskType = TaskType::Primitive;
        this->m_taskName = "SetArrowTask";
    }
    SetArrowTask(PrimitiveTask<TWorldState, TChara>::ActFunc func)
    {
        this->m_taskType = TaskType::Primitive;
        this->m_taskName = "SetArrow";
        this->SetOperator(func);
    }
    ~SetArrowTask() = default;

private:
    bool IsSatisfiedPreConditions(const TWorldState& currentState) override
    {
        //UŒ‚€”õ‚Å‚«‚½?
        return currentState.canPrepareAttack == true;
    }

    void ApplyEffectsToWorldState(TWorldState& previousState) override
    {
        //–î‚ğ”Ô‚¦‚½
        previousState.isSetArrow = true;
    }
};
