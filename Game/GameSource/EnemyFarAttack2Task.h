#pragma once
#include "EnemyBehaviorTask.h"
#include "CharacterParameter.h"
class Enemy;
class EnemyFarAttack2Task : public EnemyBehaviorTask
{
public:
	EnemyFarAttack2Task() { m_moveState = Action::START; };
	~EnemyFarAttack2Task() = default;
	void Run(Enemy* enemy);
	bool JudgeBlendRatio(CharacterParameter::BlendAnimation& animation, const bool isLoop = false);
	void JudgeAttack(Enemy* enemy, const int attackNo);
	bool JudgeAnimationRatio(Enemy* enemy, const int attackNo, const int nextAnimNo);
	bool JudgeLookTarget(Enemy* enemy);
	int  JudgeTurnChace(Enemy* enemy);

	void PrepareForStone(Enemy* enemy);
	void TurningChase(Enemy* enemy);
	bool IsTurnChase(Enemy* enemy);

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
	enum Action
	{
		START,
		LOOK_TARGET,
		RPUNCH_UPPER,
		TURN_CHACE,
		END
	};
	
	const uint32_t  kDamageRatio = 15;
	const uint32_t	kTurnChanseTimer = 70;
	const uint32_t	kAttackTimer[2] = { 50,80 };
	const float		kBlendValue = 0.05f;
	const float		kMinDirection = 22.0f;
	int			m_attackNo = 0;
	bool		m_isHit = false;
	bool		m_isTurning = false;
	VECTOR3F    m_targetPosition = {};
};

CEREAL_CLASS_VERSION(EnemyFarAttack2Task, 2);