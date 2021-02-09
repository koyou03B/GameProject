#pragma once
#include "HTN\CompoundTask.h"

template<class TWorldState, class TChara>
class AttackTask :public CompoundTask<TWorldState, TChara>
{
public:
    AttackTask()
    {
        this->m_taskType = TaskType::Compound;
        this->m_taskName = "AttackTask";
    }
    AttackTask(std::vector<std::shared_ptr<Method<TWorldState, TChara>>> methods)
    {
        this->m_taskType = TaskType::Compound;
        this->m_taskName = "AttackTask";
        this->m_methods = methods;
    }
    ~AttackTask() = default;

};
