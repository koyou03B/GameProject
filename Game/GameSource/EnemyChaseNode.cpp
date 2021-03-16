#include "EnemyChaseNode.h"

//**************************************************
// JudgePriorityRule
//	1.The player with more hit attacks
//	2.Distance to the player
//**************************************************
uint32_t EnemyChaseNode::JudgePriority(const int id)
{
	EnemyType enemyType = static_cast<EnemyType>(id);
	CharacterAI* enemy = MESSENGER.CallEnemyInstance(enemyType);
	CharacterAI* fighter = MESSENGER.CallPlayerInstance(PlayerType::Fighter);
	CharacterAI* archer = MESSENGER.CallPlayerInstance(PlayerType::Archer);

	uint32_t fighterAttackHitCount = 0;
	uint32_t archerAttackHitCount = 0;
	VECTOR3F playerPosition = {};
	int targetID = enemy->GetJudgeElement().targetID;
	PlayerType playerType = static_cast<PlayerType>(targetID);

	if (fighter)
	{
		fighterAttackHitCount = fighter->GetJudgeElement().attackHitCount;
		if (playerType == PlayerType::Fighter)
			playerPosition = fighter->GetWorldTransform().position;
	}
	if (archer)
	{
		archerAttackHitCount = archer->GetJudgeElement().attackHitCount;
		if (playerType == PlayerType::Archer)
			playerPosition = archer->GetWorldTransform().position;
	}

	if (!m_isUse)
	{
		fighter->GetJudgeElement().attackHitCount = 0;
		archer->GetJudgeElement().attackHitCount = 0;
		if (fighterAttackHitCount > archerAttackHitCount)
		{
			//	enemy->GetJudgeElement().targetID = static_cast<int>(PlayerType::Fighter);
			if (playerType != PlayerType::Fighter)
			{
				enemy->GetJudgeElement().targetID = static_cast<int>(PlayerType::Fighter);

				m_isUse = true;
				return m_priority;
			}
		}
		else
		{
			//	enemy->GetJudgeElement().targetID = static_cast<int>(PlayerType::Archer);
			if (playerType != PlayerType::Archer)
			{
				enemy->GetJudgeElement().targetID = static_cast<int>(PlayerType::Archer);
				m_isUse = true;
				return m_priority;
			}
		}
	}
	else
	{
		m_isUse = false;
		fighter->GetJudgeElement().attackHitCount = 0;
		archer->GetJudgeElement().attackHitCount = 0;
	}

	VECTOR3F enemyPosition = enemy->GetWorldTransform().position;
	float distance = ToDistVec3(playerPosition - enemyPosition);

	if (distance >= m_maxDirection)
	{
//		++m_selectCount;
		return m_priority;
	}

	return minPriority;
}