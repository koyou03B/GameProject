#pragma once
#include "HTN\CompoundTask.h"

template<class TWorldState, class TChara>
class AvoidTask :public CompoundTask<TWorldState, TChara>
{
public:
    AvoidTask()
    {
        this->m_taskType = TaskType::Compound;
        this->m_taskName = "AvoidTask";
    }
    AvoidTask(std::vector<std::shared_ptr<Method<TWorldState, TChara>>> methods)
    {
        this->m_taskType = TaskType::Compound;
        this->m_taskName = "AvoidTask";
        this->m_methods = methods;
    }
    ~AvoidTask() = default;

};
