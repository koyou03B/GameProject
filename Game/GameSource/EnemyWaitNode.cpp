#include "EnemyWaitNode.h"

//**************************************************
// JudgePriorityRule
//	1.If the exhaustion is building up
//		Cost of exhaustion
//		1.moveCost:‚Ç‚ê‚¾‚¯“®‚¢‚½‚©
//		2.attackCost:UŒ‚‚µ‚½‰ñ”
//		3.damageCost:UŒ‚‚ğó‚¯‚½‰ñ”
//		4.forgetExhaustionCost:“{‚èó‘Ô‚È‚çƒRƒXƒgŒyŒ¸
//	2.If the wrath is building up
//		Cost of wrath
//		1.damageCost:UŒ‚‚ğó‚¯‚½‰ñ”
//		2.attackHitCost:UŒ‚‚ğ“–‚Ä‚½‰ñ”
//**************************************************
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
	uint32_t maxExhaustionCost = enemy->GetEmotion().exhaustionParm.maxExhaustionCost;
	if (exhaustionCost >= maxExhaustionCost)
	{
		enemy->GetEmotion().exhaustionParm.isExhaustion = true;
		return m_priority;
	}

	if (!isWrath)
	{
		uint32_t attackHitCost = enemy->GetJudgeElement().attackHitCount * enemy->GetEmotion().wrathParm.attackWrathCost;
		enemy->GetEmotion().wrathParm.wrathCost = damageCost - attackHitCost;

		uint32_t wrathCost = enemy->GetEmotion().wrathParm.wrathCost;
		if (wrathCost >= enemy->GetEmotion().wrathParm.maxWrathCost)
		{
			enemy->GetEmotion().wrathParm.isWrath = true;
			return m_priority;
		}
	}

	return minPriority;
}