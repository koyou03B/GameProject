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
        //攻撃ポイント見つけてる
        return currentState.canFindAttackPoint == true;
    }

    void ApplyEffectsToWorldState(TWorldState& previousState) override
    {
        //攻撃準備完了,攻撃ポイントはもういらない
        previousState.canPrepareAttack = true;
        previousState.canFindAttackPoint = false;
    }
};
