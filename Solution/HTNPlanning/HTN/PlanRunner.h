#pragma once
#include "HTNPlanner.h"

#pragma region PlanRunnerについて
//役割としては
//「rootTask決まってるやん。ちょっとプランニングしてくる!」

//HTNPlannerっていうのはプランニング機能を持っている
//こいつを働かせてPlanListを構築する

//Q.currentPlanの存在理由とは
//A.アクシデントが起きた時に役立つ可能性を秘めているから
//まだ正確にこのため！っていう機能は作れてないが...。
#pragma endregion
template<class TWorldState,class TChara>
class  PlanRunner
{
public:
    PlanRunner() = default;
    ~PlanRunner() = default;

    using PlanList = std::vector<TaskBase<TWorldState, TChara>*>;

    PlanList& UpdatePlan(TWorldState& currentState)
    {
        m_currentPlan =  m_planner.Planning(currentState, m_rootTask.get());
        return m_currentPlan;
    }

    void SetRootTask(std::shared_ptr<TaskBase<TWorldState, TChara>> task)
    {
        m_rootTask = task;
    }

    void Reset()
    {
        m_currentPlan.clear();
    }

    //本来ここにこれはない
    //確認用
    void Excute(TChara* chara)
    {
        for (auto& plan : m_currentPlan)
        {
            plan->ExecuteOperator(chara);
        }
    }

private:  
    HTNPlanner<TWorldState, TChara> m_planner;
    std::shared_ptr<TaskBase<TWorldState, TChara>> m_rootTask;
    PlanList m_currentPlan;
};

