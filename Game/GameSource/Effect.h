#pragma once
#include "WorldState.h"

#pragma region Effectについて
//Effectはタスクが実行された際に
//WorldStateに与える影響のこと
//基底クラス
#pragma endregion
template<class State>
class Effect
{
public:
	Effect() = default;
	~Effect() = default;

	inline void ApplyTo(State& state)
	{
		if (m_status != WorldState::Status::MaxStatus)
			state.SetStatus(m_status, m_value);
		else
			state.SetStatus(m_statusForTemp, m_value);
	}
private:
	WorldState::Status m_status;
	int m_statusForTemp;
	bool m_value;
};