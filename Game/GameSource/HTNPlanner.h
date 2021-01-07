#pragma once
#include <queue>
#include <memory>
#include <assert.h>
#include "PlanList.h"
#include "WorldState.h"
#include "ITask.h"

class HTNPlanner
{
public:
	HTNPlanner() = default;
	~HTNPlanner() = default;

	template<class State>
	PlanList& Planning(WorldState& state, std::shared_ptr<ITask<State>> rootTask);

private:
	PlanList m_plaList;
};

template<class State>
inline PlanList& HTNPlanner::Planning(WorldState& state, std::shared_ptr<ITask<State>> rootTask)
{
	m_plaList.Clear();

	if (!rootTask->TryPlanTask(state, m_plaList))
		assert("Why can't I create a PlanList! It's a bad Precondition, or WorldState.");

	return m_plaList;
}
