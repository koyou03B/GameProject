#pragma once
#include <string>
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

	inline void ToSave(std::string name)
	{
		std::ofstream ofs;
		ofs.open((std::string("../Asset/Binary/HTN/Effect/") + name + ".bin").c_str(), std::ios::binary);
		cereal::BinaryOutputArchive o_archive(ofs);
		o_archive(*this);
	}

	inline void ToLoad(std::string name, int value)
	{
		if (PathFileExistsA((std::string("../Asset/Binary/HTN/Effect/") + name + ".bin").c_str()))
		{
			std::ofstream ifs;
			ifs.open((std::string("../Asset/Binary/HTN/Effect/") + name + ".bin").c_str(), std::ios::binary);
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