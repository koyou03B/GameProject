#pragma once
#include "EnemyBehaviorTask.h"
#include "CharacterParameter.h"
class Enemy;
class EnemyTargetTurnTask : public EnemyBehaviorTask
{
public:
	EnemyTargetTurnTask() = default;
	~EnemyTargetTurnTask() = default;
	void Run(Enemy* enemy);
	bool JudgeBlendRatio(CharacterParameter::BlendAnimation& animation);

	uint32_t JudgePriority(const int id, const VECTOR3F playerPos) override;

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
		ofs.open((std::string("../Asset/Binary/Enemy/BehaviorTask/") + m_taskName + ".bin").c_str()
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
				m_parentNodeName
			);
		}
		else
		{
			archive
			(
				m_taskName,
				m_coolTimer,
				m_priority,
				m_parentNodeName
			);
		}
	}

private:
	int m_targetID = 0;
	bool m_isLockOn = false;
	const uint32_t m_moveAnimationTime = 70;
};

CEREAL_CLASS_VERSION(EnemyTargetTurnTask, 2);