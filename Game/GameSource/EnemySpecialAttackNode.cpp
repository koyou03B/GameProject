#include "EnemySpecialAttackNode.h"

uint32_t EnemySpecialAttackNode::JudgePriority(const int id)
{
	std::shared_ptr<CharacterAI> enemy = MESSENGER.CallEnemyInstance(id);
	bool isWrath = enemy->GetEmotion().wrathParm.isWrath;

	if (isWrath)
	{
		uint32_t attackCount = enemy->GetJudgeElement().attackCount;
		if(attackCount > m_maxAttackCount)
		return m_priority;
	}

	return minPriority;
}
