#pragma once
#include <memory>
#include "HTNPlanner.h"
#include "ITask.h"

template <class State>
class HTNPlanRunner
{
public:
	HTNPlanRunner() = default;
	~HTNPlanRunner() = default;

	PlanList& UpdatePlan(WorldState& worldState);

private:
	HTNPlanner m_planner;
	std::shared_ptr<ITask<State>> m_rootTask;
};

template<class State>
inline PlanList& HTNPlanRunner<State>::UpdatePlan(WorldState& worldState)
{
	return m_planner.Planning(worldState, m_rootTask);
}
