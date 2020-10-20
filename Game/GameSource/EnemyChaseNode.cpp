#include "EnemyChaseNode.h"

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
				return m_priority;
		
			}
		}
	}

	return minPriority;
}