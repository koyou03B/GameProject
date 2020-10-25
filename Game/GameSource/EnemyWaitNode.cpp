#include "EnemyWaitNode.h"

uint32_t EnemyWaitNode::JudgePriority(const int id)
{
	std::shared_ptr<CharacterAI> enemy = MESSENGER.CallEnemyInstance(id);

	uint32_t moveCost   = enemy->GetJudgeElement().moveCount * enemy->GetEmotion().exhaustionParm.moveExhaustionCost;
	uint32_t attackCost = enemy->GetJudgeElement().attackCount * enemy->GetEmotion().exhaustionParm.attackExhaustionCost;
	uint32_t damageCost = enemy->GetJudgeElement().damageCount * enemy->GetEmotion().exhaustionParm.damageExhaustionCost;
	enemy->GetEmotion().exhaustionParm.exhaustionCost = moveCost + attackCost + damageCost;

	bool isWrath = enemy->GetEmotion().wrathParm.isWrath;
	if (isWrath)
		enemy->GetEmotion().exhaustionParm.exhaustionCost -= enemy->GetEmotion().exhaustionParm.forgetExhaustionCost;

	uint32_t exhaustionCost = enemy->GetEmotion().exhaustionParm.exhaustionCost;

	if (exhaustionCost >= enemy->GetEmotion().exhaustionParm.maxExhaustionCost)
		return m_priority;

	return minPriority;
}