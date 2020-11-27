#include "EnemyChaseNode.h"

//**************************************************
// JudgePriorityRule
//	1.The player with more hit attacks
//	2.Distance to the player
//**************************************************
uint32_t EnemyChaseNode::JudgePriority(const int id)
{
	auto players = MESSENGER.CallPlayersInstance();
	std::shared_ptr<CharacterAI> enemy = MESSENGER.CallEnemyInstance(id);
	int targetID = enemy->GetJudgeElement().targetID;
	uint32_t targetAttackHitCount = players.at(targetID)->GetJudgeElement().attackHitCount;

	int playerCount = static_cast<int>(players.size());
	for (int i = 0; i < playerCount; ++i)
	{
		if (i != targetID)
		{
			uint32_t attackHitCount = players.at(i)->GetJudgeElement().attackHitCount;
			if (targetAttackHitCount < attackHitCount)
			{
				enemy->GetJudgeElement().targetID = i;
				return m_priority;
			}
		}
	}

	if (m_selectCount > 1)
	{
		m_selectCount = 0;
		return minPriority;
	}

	VECTOR3F playerPosition = players.at(targetID)->GetWorldTransform().position;
	VECTOR3F enemyPosition = enemy->GetWorldTransform().position;

	float distance = ToDistVec3(playerPosition - enemyPosition);

	if (distance >= m_maxDirection)
	{
		++m_selectCount;
		return m_priority;
	}



	return minPriority;
}