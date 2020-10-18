#include "EnemyWaitNode.h"

uint32_t EnemyWaitNode::JudgePriority(const int id)
{
	std::shared_ptr<CharacterAI> enemy = MESSENGER.CallEnemyInstance(id);

	AIParameter::Emotion emotion = enemy->GetEmotion();
	AIParameter::JudgeElement judgeElement = enemy->GetJudgeElement();
	uint32_t moveCost   = judgeElement.moveCount * emotion.exhaustionParm.moveExhaustionCost;
	uint32_t attackCost = judgeElement.attackCount * emotion.exhaustionParm.attackExhaustionCost;
	uint32_t damageCost = judgeElement.damageCount * emotion.exhaustionParm.damageExhaustionCost;
	emotion.exhaustionParm.exhaustionCost = moveCost + attackCost + damageCost;

	if (emotion.wrathParm.isWrath)
		emotion.exhaustionParm.exhaustionCost -= emotion.exhaustionParm.forgetExhaustionCost;

	uint32_t exhaustionCost = emotion.exhaustionParm.exhaustionCost;

	if (exhaustionCost >= emotion.exhaustionParm.maxExhaustionCost)
		return m_priority;

	return minPriority;
}