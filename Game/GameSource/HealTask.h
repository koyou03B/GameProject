#pragma once
#pragma once
#include "HTN\PrimitiveTask.h"

template<class TWorldState, class TChara>
class HealTask :public PrimitiveTask<TWorldState, TChara>
{
public:
    HealTask()
    {
        this->m_taskType = TaskType::Primitive;
        this->m_taskName = "HealTask";
    }
    HealTask(PrimitiveTask<TWorldState, TChara>::ActFunc func)
    {
        this->m_taskType = TaskType::Primitive;
        this->m_taskName = "HealTask";
        this->SetOperator(func);
    }
    ~HealTask() = default;

private:
    bool IsSatisfiedPreConditions(const TWorldState& currentState) override
    {
        return  true;
    }

    void ApplyEffectsToWorldState(TWorldState& previousState) override
    {
        //‰ñ•œ‚Å‚«‚½
        previousState._canRecover = false;
    }
};
