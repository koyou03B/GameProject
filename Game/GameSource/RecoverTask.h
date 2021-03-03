#pragma once
#include "HTN\CompoundTask.h"

template<class TWorldState, class TChara>
class RecoverTask :public CompoundTask<TWorldState, TChara>
{
public:
    RecoverTask()
    {
        this->m_taskType = TaskType::Compound;
        this->m_taskName = "RecoverTask";
    }
    RecoverTask(std::vector<std::shared_ptr<Method<TWorldState, TChara>>> methods)
    {
        this->m_taskType = TaskType::Compound;
        this->m_taskName = "RecoverTask";
        this->m_methods = methods;
    }
    ~RecoverTask() = default;

};
