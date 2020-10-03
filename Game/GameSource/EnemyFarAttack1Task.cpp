#include "EnemyFarAttack1Task.h"
#include "MessengTo.h"
#include "Enemy.h"

void EnemyFarAttack1Task::Run(Enemy* enemy)
{
}

uint32_t EnemyFarAttack1Task::JudgePriority(const int id)
{
	std::shared_ptr<CharacterAI> enemy = MESSENGER.CallEnemyInstance(id);
	uint32_t attackCount = static_cast<uint32_t>(enemy->GetJudgeElement().attackCount);
	uint32_t attackCountRatio = static_cast<uint32_t>(enemy->GetJudgeElement().attackCountRatio);

	if (attackCount > attackCountRatio)
		return m_priority;

	return minPriority;
}
