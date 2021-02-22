#include "EnemyUnSpecialAttackNode.h"

uint32_t EnemyUnSpecialAttackNode::JudgePriority(const int id)
{
	EnemyType type = static_cast<EnemyType>(id);
	CharacterAI* enemy = MESSENGER.CallEnemyInstance(type);
	bool isWrath = enemy->GetEmotion().wrathParm.isWrath;

	if (!isWrath)
		return m_priority;

	return minPriority;
}
