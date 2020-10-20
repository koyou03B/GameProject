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
	bool JudgeBlendRatio(CharacterParameter::BlendAnimation& animation);
	bool JudgeAnimationRatio(Enemy* enemy, const int attackNo, const int nextAnimNo);
	void JudgeAttack(Enemy* enemy, const int attackNo);
	void TurningChase(Enemy* enemy);

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
		TURNING,
		RUN,
		BLNEDNOW,
		RIGTH_UPPER,
		END
	};
	const uint32_t	kTurningTimer = 130;
	const uint32_t	kRowlingTimer[2] = { 30,120 };
	const uint32_t	kAttackTimer[2] = { 30,90 };
	const uint32_t	kMoveTimer[2] = { 10,40 };
	const int		kCollisionNo[2] = { 1,4 };
	const float		kFourSecond = 240.0f;
	const float		kChaseMaxTimer = 200.0f;
	const float		kBlendValue = 0.045f;
	
	int m_attackNo = 0;
	int m_targetID = 0;
	float m_chaseTimer = 0.0f;
	float m_speedToTarget = 0.0f;
	bool m_isNear = false;
	bool m_isTurning = false;
	VECTOR3F m_nVecToTarget = {};
	VECTOR3F m_targetPosition = {};
};

CEREAL_CLASS_VERSION(EnemyFarAttack1Task, 2);