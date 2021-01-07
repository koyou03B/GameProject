#include "PrimitiveTask.h"

//template<class State>
//bool PrimitiveTask<State>::TryPlanTask(State& state, PlanList& planList)
//{
//	size_t count = m_preconditions.size();
//	for (size_t i = 0; i < count; ++i)
//	{
//		if (m_preconditions[i].CheckPreCondition(state))
//			return false;
//	}
//
//	count = m_effects.size();
//	for (size_t i = 0; i < count; ++i)
//	{
//		m_effects[i].ApplyTo(state);
//	}
//
//	planList.AddOperator(m_operatorID);
//
//	return true;
//}