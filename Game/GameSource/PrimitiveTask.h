#pragma once
#include <vector>
#include <string>
#include <memory>
#include "ITask.h"
#include "ArcherWorldState.h"
#include "PreCondition.h"
#include "Effect.h"

#pragma region PrimitiveTask�ɂ���
//PrimitiveTask��
//���E�ɉe����^����s����\������
//����3�̗v�f�łł��Ă���
//PreCondition	�^�X�N�����s���邽�߂ɕK�v�ȏ���
//Operator		�I�y���[�^�͎��ۂɍs���s��
//Effect		�I�y���[�^�̍s���Ń��[���h�X�e�[�g���ǂ̂悤�ɕς�邩
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
