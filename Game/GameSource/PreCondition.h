#pragma once
#include <string>
#include "WorldState.h"

#pragma region Preconditionについて
//タスクを実行するための条件
//基底クラス
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

	inline void ToSave(std::string name)
	{
		std::ofstream ofs;
		ofs.open((std::string("../Asset/Binary/HTN/PreCondition/") + name + ".bin").c_str(), std::ios::binary);
		cereal::BinaryOutputArchive o_archive(ofs);
		o_archive(*this);
	}

	inline void ToLoad(std::string name, int value)
	{
		if (PathFileExistsA((std::string("../Asset/Binary/HTN/PreCondition/") + name + ".bin").c_str()))
		{
			std::ofstream ifs;
			ifs.open((std::string("../Asset/Binary/HTN/PreCondition/") + name + ".bin").c_str(), std::ios::binary);
			cereal::BinaryInputArchive i_archive(ifs);
			i_archive(*this);
		}
	}

	template<class T>
	void serialize(T& archive, const std::uint32_t version)
	{

		if (0 <= version)
		{
			archive
			(
				m_status,
				m_statusForTemp,
				m_value
			);
		}
		else
		{
			archive
			(
				m_status,
				m_statusForTemp,
				m_value
			);
		}
	}

private:
	WorldState::Status m_status;
	int m_statusForTemp;
	bool m_value;
};