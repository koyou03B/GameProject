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
	int targetID = enemy->GetJudgeElement().targetID;
	PlayerType playerType = static_cast<PlayerType>(targetID);
	CharacterAI* player = MESSENGER.CallPlayerInstance(playerType);

	uint32_t targetAttackHitCount = player->GetJudgeElement().attackHitCount;

	int count = static_cast<int>(PlayerType::End);
	for (int i = 0; i < count; ++i)
	{
		if (i != targetID)
		{
			playerType = static_cast<PlayerType>(i);
			CharacterAI* otherPlayer = MESSENGER.CallPlayerInstance(playerType);
			uint32_t attackHitCount = otherPlayer->GetJudgeElement().attackHitCount;
			if (targetAttackHitCount < attackHitCount)
			{
				enemy->GetJudgeElement().targetID = i;
				return m_priority;
			}
		}
	}

	VECTOR3F playerPosition = player->GetWorldTransform().position;
	VECTOR3F enemyPosition = enemy->GetWorldTransform().position;

	float distance = ToDistVec3(playerPosition - enemyPosition);

	if (distance >= m_maxDirection)
	{
		++m_selectCount;
		return m_priority;
	}

	return minPriority;
}