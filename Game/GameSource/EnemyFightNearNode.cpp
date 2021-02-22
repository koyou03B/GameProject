#include "EnemyFightNearNode.h"

uint32_t EnemyFightNearNode::JudgePriority(const int id)
{
	EnemyType enemyType = static_cast<EnemyType>(id);
	CharacterAI* enemy = MESSENGER.CallEnemyInstance(enemyType);
	int targetID = enemy->GetJudgeElement().targetID;
	PlayerType playerType = static_cast<PlayerType>(targetID);
	CharacterAI* player = MESSENGER.CallPlayerInstance(playerType);

	VECTOR3F playerPosition = player->GetWorldTransform().position;
	VECTOR3F enemyPosition = enemy->GetWorldTransform().position;

	float direction = ToDistVec3(playerPosition - enemyPosition);

	if (direction < m_maxDirection)
		return m_priority;

	return minPriority;
}