#pragma once
#include "HTN/PrimitiveTask.h"

template<class TWorldState, class TChara>
class FindDATask :public PrimitiveTask<TWorldState, TChara>
{
public:
    FindDATask()
    {
        this->m_taskType = TaskType::Primitive;
        this->m_taskName = "FindDATask";
    }
    FindDATask(PrimitiveTask<TWorldState, TChara>::ActFunc func)
    {
        this->m_taskType = TaskType::Primitive;
        this->m_taskName = "FindDATask";
        this->SetOperator(func);
    }
    ~FindDATask() = default;

private:
    bool IsSatisfiedPreConditions(const TWorldState& currentState) override
    {
        //���OK��Ԃ��̂��I�����̈��
        return  true;
    }

    void ApplyEffectsToWorldState(TWorldState& previousState) override
    {
        //�U��Point����
        previousState._canFindDirectionToAvoid = true;
        previousState._canAvoid = true;
    }
};
