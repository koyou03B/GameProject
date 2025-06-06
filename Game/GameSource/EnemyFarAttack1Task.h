#pragma once
#include "EnemyBehaviorTask.h"
#include "CharacterParameter.h"
class Enemy;
class EnemyFarAttack1Task : public EnemyBehaviorTask
{
public:
	EnemyFarAttack1Task() { m_moveState = Action::START; };
	~EnemyFarAttack1Task() = default;
	void Run(Enemy* enemy);	
	bool JudgeBlendRatio(CharacterParameter::BlendAnimation& animation, const bool isLoop = false);
	void JudgeAttack(Enemy* enemy, const int attackNo);
	bool JudgeAnimationRatio(Enemy* enemy, const int attackNo, const int nextAnimNo);
	int  JudgeTurnChace(Enemy* enemy);
	void JudgeVectorDirection(Enemy* enemy);

	void BackFlipTurn(Enemy* enemy);
	bool AttackMove(Enemy* enemy);
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
		BACK_FLIP,
		RUN_TURN_ATTACK,
		TURN_CHACE,
		END
	};

	const uint32_t	kTurningTimer = 130;
	const uint32_t	kAttackSlowTimer = 30;
	const uint32_t	kTurnChanseTimer = 70;
	const uint32_t	kAttackTimer[2] = { 60,110 };
	const float		kAccel = 80.0f;

	int		m_attackNo = 0;
	int		m_targetID = 0;
	float   m_blendValue = 0.0f;
	float	m_accel = 90.0f;
	bool	m_isNear = false;
	bool	m_isHit = false;
	bool	m_isTurning = false;
	bool	m_setTarget = false;
	bool    m_hasFinishedAttack = false;
	VECTOR3F m_nVecToTarget = {};
	VECTOR3F m_targetPosition = {};
};

CEREAL_CLASS_VERSION(EnemyFarAttack1Task, 2);