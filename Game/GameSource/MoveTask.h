#pragma once
#include "HTN\PrimitiveTask.h"

template<class TWorldState, class TChara>
class MoveTask :public PrimitiveTask<TWorldState, TChara>
{
public:
    MoveTask()
    {
        this->m_taskType = TaskType::Primitive;
        this->m_taskName = "MoveTask";
    }
    MoveTask(PrimitiveTask<TWorldState, TChara>::ActFunc func)
    {
        this->m_taskType = TaskType::Primitive;
        this->m_taskName = "MoveTask";
        this->SetOperator(func);
    }
    ~MoveTask() = default;

private:
    bool IsSatisfiedPreConditions(const TWorldState& currentState) override
    {
        //�U���|�C���g�����Ă�
        return currentState.canFindAttackPoint == true;
    }

    void ApplyEffectsToWorldState(TWorldState& previousState) override
    {
        //�U����������,�U���|�C���g�͂�������Ȃ�
        previousState.canPrepareAttack = true;
        previousState.canFindAttackPoint = false;
    }
};
