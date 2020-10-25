#include "EnemyUnSpecialAttackNode.h"

uint32_t EnemyUnSpecialAttackNode::JudgePriority(const int id)
{
	std::shared_ptr<CharacterAI> enemy = MESSENGER.CallEnemyInstance(id);
	bool isWrath = enemy->GetEmotion().wrathParm.isWrath;

	if (!isWrath)
		return m_priority;

	return minPriority;
}
