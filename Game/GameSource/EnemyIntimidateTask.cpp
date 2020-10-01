#include "EnemyIntimidateTask.h"
#include "MessengTo.h"
#include "Enemy.h"

void EnemyIntimidateTask::Run(Enemy* enemy)
{
}

uint32_t EnemyIntimidateTask::JudgePriority(const int id)
{
	std::shared_ptr<CharacterAI> enemy = MESSENGER.CallEnemyInstance(id);
	//**************************
	// MoveCost,AttackCost
	//**************************
	AIParameter::Emotion emotion = enemy->GetEmotion();
	AIParameter::JudgeElement judgeElement = enemy->GetJudgeElement();

	if (emotion.wrathParm.isWrath)
		return minPriority;

	uint32_t attackHitCost = judgeElement.attackHitCount * emotion.wrathParm.attackWrathCost;
	uint32_t damageCost = judgeElement.damageCount * emotion.wrathParm.damageWrathCost;

	uint32_t wrathCost = damageCost - attackHitCost;

	if (wrathCost >= emotion.wrathParm.maxWrathCost)
		return m_priority;

	return minPriority;
}
