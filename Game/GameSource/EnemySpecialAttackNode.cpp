#include "EnemySpecialAttackNode.h"

uint32_t EnemySpecialAttackNode::JudgePriority(const int id)
{
	EnemyType type = static_cast<EnemyType>(id);
	CharacterAI* enemy = MESSENGER.CallEnemyInstance(type);

	bool isWrath = enemy->GetEmotion().wrathParm.isWrath;

	if (isWrath)
	{
		uint32_t attackCount = enemy->GetJudgeElement().attackCount;
		if(attackCount > m_maxAttackCount)
		return m_priority;
	}

	return minPriority;
}
