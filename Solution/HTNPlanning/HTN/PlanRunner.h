#pragma once
#include "HTNPlanner.h"

#pragma region PlanRunner�ɂ���
//�����Ƃ��Ă�
//�urootTask���܂��Ă���B������ƃv�����j���O���Ă���!�v

//HTNPlanner���Ă����̂̓v�����j���O�@�\�������Ă���
//�����𓭂�����PlanList���\�z����

//Q.currentPlan�̑��ݗ��R�Ƃ�
//A.�A�N�V�f���g���N�������ɖ𗧂\�����߂Ă��邩��
//�܂����m�ɂ��̂��߁I���Ă����@�\�͍��ĂȂ���...�B
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

    //�{�������ɂ���͂Ȃ�
    //�m�F�p
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

