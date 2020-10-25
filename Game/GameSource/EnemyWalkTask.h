#pragma once
#include "EnemyBehaviorTask.h"
#include "CharacterParameter.h"

class Enemy;
class EnemyWalkTask : public EnemyBehaviorTask
{
public:
	EnemyWalkTask() { m_moveState = Action::START; };
	~EnemyWalkTask() = default;
	void Run(Enemy* enemy);
	bool JudgeBlendRatio(CharacterParameter::BlendAnimation& animation);
	bool IsTurnChase(Enemy* enemy);

	int  JudgeTurnChace(Enemy* enemy);
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
	enum Action
	{
		START,
		WALK,
		END
	};
	const float kWalkTimer = 5.0f;
	const float		kFIveSecond = 300.0f;
	const float	kBlendValue = 0.055f;
	const float kTurnValue = 0.01f;
	float m_walkTime = 0;
};

CEREAL_CLASS_VERSION(EnemyWalkTask, 2);
