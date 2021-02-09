#pragma once
#include "HTN/PrimitiveTask.h"

template<class TWorldState, class TChara>
class FindAPTask :public PrimitiveTask<TWorldState, TChara>
{
public:
    FindAPTask()
    {
        this->m_taskType = TaskType::Primitive;
        this->m_taskName = "FindAttackPointTask";
    }
    FindAPTask(PrimitiveTask<TWorldState, TChara>::ActFunc func)
    {
        this->m_taskType = TaskType::Primitive;
        this->m_taskName = "FindAttackPointTask";
        this->SetOperator(func);
    }
    ~FindAPTask() = default;

private:
    bool IsSatisfiedPreConditions(const TWorldState& currentState) override
    {
        //í‚ÉOK‚ğ•Ô‚·‚Ì‚à‘I‘ğˆ‚Ìˆê‚Â
        return  true;
    }

    void ApplyEffectsToWorldState(TWorldState& previousState) override
    {
        //UŒ‚Point”­Œ©
        previousState.canFindAttackPoint = true;
    }
};
