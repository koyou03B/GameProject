#pragma once
#include "EnemyBehaviorTask.h"
#include "CharacterParameter.h"
class Enemy;
class EnemyNearSpecialAttack0Task : public EnemyBehaviorTask
{
public:
	EnemyNearSpecialAttack0Task() { m_moveState = Action::START; };
	~EnemyNearSpecialAttack0Task() = default;
	void Run(Enemy* enemy);
	bool JudgeBlendRatio(CharacterParameter::BlendAnimation& animation);
	bool JudgeAnimationRatio(Enemy* enemy, const int attackNo, const int nextAnimNo);
	void JudgeAttack(Enemy* enemy, const int attackNo);
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
		WRATH_ATTACK,
		END
	};
	const uint32_t		kAttackTimer[2] = { 140,160 };
	const int			kCollisionNo = 3;
	const float			kBlendValue = 0.025f;
	int m_attackNo = 0;

};

CEREAL_CLASS_VERSION(EnemyNearSpecialAttack0Task, 2);