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
        //����������������Ă�
        return currentState.canAvoid == true;
    }

    void ApplyEffectsToWorldState(TWorldState& previousState) override
    {
        //����������͂�������Ȃ�
       previousState.canFindDirectionToAvoid = false;
       previousState.canAvoid = false;
    }
};
