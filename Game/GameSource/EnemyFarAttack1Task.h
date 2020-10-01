#pragma once
#include "EnemyBehaviorTask.h"

class Enemy;
class EnemyFarAttack1Task : public EnemyBehaviorTask
{
public:
	EnemyFarAttack1Task() = default;
	~EnemyFarAttack1Task() = default;
	void Run(Enemy* enemy);
	uint32_t JudgePriority(const int id);

	void LoadOfBinaryFile(std::string taskName)
	{
		if (PathFileExistsA((std::string("../Asset/Binary/Enemy/BehaviorTask/") + taskName + ".bin").c_str()))
		{
			std::ifstream ifs;
			ifs.open((std::string("../Asset/Binary/Enemy/BehaviorTask/") + taskName + ".bin").c_str()
				, std::ios::binary);
			cereal::BinaryInputArchive i_archive(ifs);
			i_archive(*this);
		}
	}

	void SaveOfBinaryFile()
	{
		std::ofstream ofs;
		ofs.open((std::string("../Asset/Binary/Enemy/BehaviorTask") + m_taskName + ".bin").c_str()
			, std::ios::binary);
		cereal::BinaryOutputArchive o_archive(ofs);
		o_archive(*this);
	}

	template<class T>
	void serialize(T& archive, const std::uint32_t version)
	{
		if (m_serialVersion <= version)
		{
			archive
			(
				m_taskName,
				m_coolTimer,
				m_priority,
				m_attackRatio
			);
		}
		else
		{
			archive
			(
				m_taskName,
				m_coolTimer,
				m_priority,
				m_attackRatio
			);
		}
	}
private:
	uint32_t m_attackRatio;
};

CEREAL_CLASS_VERSION(EnemyFarAttack1Task, 1);