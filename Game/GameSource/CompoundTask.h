#pragma once
#include <vector>
#include "Method.h"
#include "PrimitiveTask.h"
#include "ArcherWorldState.h"

#pragma region CompoundTaskについて
//タスクが達成すべき結果に
//至る方法を記述したMethodを複数持つ
#pragma endregion
template<class State>
class CompoundTask : public ITask<State>
{
public:
	CompoundTask() = default;
	~CompoundTask() = default;

	bool TryPlanTask(State& state, PlanList& planList) override
	{
		size_t count = m_methods.size();
		for (size_t i = 0; i < count; ++i)
		{
			if (!m_methods[i]->CheckPreCondition(state))
				continue;

			PlanList currentPlanList = planList;
			State currentState = state;
			if (!m_methods[i]->TryPlanSubTasks(state, planList))
			{
				planList = currentPlanList;
				state = currentState;
				continue;
			}

			return true;
		}

		return false;
	}

private:
	std::string m_taskName;
	std::vector<std::shared_ptr<Method<State>>> m_methods;

};
