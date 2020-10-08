#include "EnemyFarAttack1Task.h"
#include "MessengTo.h"
#include "Enemy.h"

void EnemyFarAttack1Task::Run(Enemy* enemy)
{
}

uint32_t EnemyFarAttack1Task::JudgePriority(const int id)
{
	std::shared_ptr<CharacterAI> enemy = MESSENGER.CallEnemyInstance(id);
	uint32_t attackCount = enemy->GetJudgeElement().attackCount;
	uint32_t attackCountValue = enemy->GetStandardValue().attackCountValue;

	if (attackCount > attackCountValue)
		return m_priority;

	return minPriority;
}
