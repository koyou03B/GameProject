#include "EnemyRestTask.h"
#include "MessengTo.h"
#include "Enemy.h"

void EnemyRestTask::Run(Enemy* enemy)
{

}

uint32_t EnemyRestTask::JudgePriority(const int id)
{
	std::shared_ptr<CharacterAI> enemy = MESSENGER.CallEnemyInstance(id);
	AIParameter::Emotion emotion = enemy->GetEmotion();

	uint32_t exhaustionCost = emotion.exhaustionParm.exhaustionCost;
	
	if (exhaustionCost >= emotion.exhaustionParm.maxExhaustionCost)
		return m_priority;
	
	return minPriority;
}
