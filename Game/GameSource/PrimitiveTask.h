#pragma once
#include <vector>
#include <string>
#include <memory>
#include "ITask.h"
#include "ArcherWorldState.h"
#include "PreCondition.h"
#include "Effect.h"

#pragma region PrimitiveTaskについて
//PrimitiveTaskは
//世界に影響を与える行動を表すもの
//次の3つの要素でできている
//PreCondition	タスクを実行するために必要な条件
//Operator		オペレータは実際に行う行動
//Effect		オペレータの行動でワールドステートがどのように変わるか
#pragma endregion

template <class State>
class PrimitiveTask : public ITask<State>
{
public:
	PrimitiveTask() = default;
	~PrimitiveTask() = default;

	bool TryPlanTask(State& state, PlanList& planList) override
	{
		size_t count = m_preconditions.size();
		for (size_t i = 0; i < count; ++i)
		{
			if (m_preconditions[i].CheckPreCondition(state))
				return false;
		}

		count = m_effects.size();
		for (size_t i = 0; i < count; ++i)
		{
			m_effects[i].ApplyTo(state);
		}

		planList.AddOperator(m_operatorID);

		return true;
	}

private:
	std::string m_taskName;
	std::vector<std::shared_ptr<PreCondition<State>>>	m_preconditions;
	std::vector<std::shared_ptr<Effect<State>>>			m_effects;
	uint32_t m_operatorID;
};
