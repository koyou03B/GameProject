#pragma once
#include <vector>
#include <memory>
#include "ITask.h"
#include "ArcherWorldState.h"
#include "PreCondition.h"
#include "PrimitiveTask.h"

#pragma region Methodについて
//Methodは自分が選ばれるための
//コンディションと選ばれた時の
//Taskのリストを持つ
//CompoundTaskがMethodを持つ
#pragma endregion
template<class State>
class Method
{
public:
	Method() = default;
	~Method() = default;

	void Add(ITask<State>* state)
	{
		std::shared_ptr<ITask<State>> uState(state);
		m_subTasks.push_back(uState);
	}

	bool CheckPreCondition(State state)
	{
		size_t count = m_preconditions.size();
		for (size_t i = 0; i < count; ++i)
		{
			if (!m_preconditions[i]->CheckPreCondition(state))
				return  false;
		}
		return true;
	}

	bool TryPlanSubTasks(State& state, PlanList& planList)
	{
		size_t count = m_subTasks.size();
		for (size_t i = 0; i < count; ++i)
		{
			if (!m_subTasks[i]->TryPlanTask(state, planList))
			{
				return false;
			}
		}

		return true;
	}

public:
	std::vector<std::shared_ptr<PreCondition<State>>> m_preconditions;
	std::vector<std::shared_ptr<ITask<State>>> m_subTasks;
	//std::vector<ITask<State>*> m_subTasks;
	//std::shared_ptr<ITask<State>> m_subTasks;
};