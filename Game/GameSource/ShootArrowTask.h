#pragma once
#include "HTN\PrimitiveTask.h"

template<class TWorldState, class TChara>
class ShootArrowTask :public PrimitiveTask<TWorldState, TChara>
{
public:
    ShootArrowTask()
    {
        this->m_taskType = TaskType::Primitive;
        this->m_taskName = "ShootArrowTask";
    }
    ShootArrowTask(PrimitiveTask<TWorldState, TChara>::ActFunc func)
    {
        this->m_taskType = TaskType::Primitive;
        this->m_taskName = "ShootArrowTask";
        this->SetOperator(func);
    }
    ~ShootArrowTask() = default;

private:
    bool IsSatisfiedPreConditions(const TWorldState& currentState) override
    {
        //–î‚Í”Ô‚¦‚Ä‚¢‚ÄUŒ‚€”õ‚Å‚«‚½
        return  currentState._isSetArrow == true &&
            currentState._canPrepareAttack == true;
    }

    void ApplyEffectsToWorldState(TWorldState& previousState) override
    {
        //–î‚Í‚È‚¢‚Å‚·UŒ‚€”õ‚à‚µ‚Ä‚Ü‚¹‚ñ
        previousState._isSetArrow = false;
        previousState._canPrepareAttack = false;
        previousState._canAttack = true;
    }
};
