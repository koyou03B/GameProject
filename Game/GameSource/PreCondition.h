#pragma once
#include "WorldState.h"

#pragma region Precondition�ɂ���
//�^�X�N�����s���邽�߂̏���
//���N���X
#pragma endregion
template<class State>
class PreCondition
{
public:
	PreCondition() = default;
	~PreCondition() = default;

	inline bool CheckPreCondition(State& state)
	{
		if (m_status != WorldState::Status::MaxStatus)
			return state.GetStatus(m_status) == m_value;

		return  state.GetStatus(m_statusForTemp) == m_value;
	}

private:
	WorldState::Status m_status;
	int m_statusForTemp;
	bool m_value;
};