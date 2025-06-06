#pragma once
#pragma once
#include "HTN\PrimitiveTask.h"

template<class TWorldState, class TChara>
class AvoidingTask :public PrimitiveTask<TWorldState, TChara>
{
public:
    AvoidingTask()
    {
        this->m_taskType = TaskType::Primitive;
        this->m_taskName = "AvoidingTask";
    }
    AvoidingTask(PrimitiveTask<TWorldState, TChara>::ActFunc func)
    {
        this->m_taskType = TaskType::Primitive;
        this->m_taskName = "AvoidingTask";
        this->SetOperator(func);
    }
    ~AvoidingTask() = default;

private:
    bool IsSatisfiedPreConditions(const TWorldState& currentState) override
    {
        //避ける方向を見つけてる
        return currentState._canAvoid == true;
    }

    void ApplyEffectsToWorldState(TWorldState& previousState) override
    {
        //避ける方向はもういらない
       previousState._canFindDirectionToAvoid = false;
       previousState._canAvoid = false;
    }
};
