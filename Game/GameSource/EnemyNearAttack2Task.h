#pragma once
#include "EnemyBehaviorTask.h"
#include "CharacterParameter.h"
class Enemy;
class EnemyNearAttack2Task : public EnemyBehaviorTask
{
public:
	EnemyNearAttack2Task() { m_moveState = Action::START; };
	~EnemyNearAttack2Task() = default;
	void Run(Enemy* enemy);	
	void JudgeAttack(Enemy* enemy, const int attackNo);

	bool JudgeBlendRatio(CharacterParameter::BlendAnimation& animation);
	bool JudgeAnimationRatio(Enemy* enemy, const int attackNo, const int nextAnimNo);
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
		RIGHT_PUNCH_LOWER,
		TURN_CHACE,
		END
	};
	const uint32_t		kAttackTimer[2] = { 65,90 };
	const uint32_t		kTurnChanseTimer = 70;
	const uint32_t		kAttackSlowTimer = 60;
	const int			kCollisionNo = 1;
	const int			kRestValue = 4;
	const float			kBlendValue = 0.055f;	
	const float			kMinDirection = 20.0f;

	int					m_attackNo = 0;
	VECTOR3F			m_targetPosition = {};
};

CEREAL_CLASS_VERSION(EnemyNearAttack2Task, 2);