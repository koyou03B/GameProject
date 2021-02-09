#pragma once
#include "HTN\CompoundTask.h"

template<class TWorldState, class TChara>
class PrepareAttackTask :public CompoundTask<TWorldState, TChara>
{
public:
    PrepareAttackTask()
    {
        this->m_taskType = TaskType::Compound;
        this->m_taskName = "PrepareAttackTask";
    }
    PrepareAttackTask(std::vector<std::shared_ptr<Method<TWorldState, TChara>>> methods)
    {
        this->m_taskType = TaskType::Compound;
        this->m_taskName = "PrepareAttackTask";
        this->m_methods = methods;
    }
    ~PrepareAttackTask() = default;

};
