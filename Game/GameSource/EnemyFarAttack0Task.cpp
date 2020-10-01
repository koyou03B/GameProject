#include "EnemyFarAttack0Task.h"
#include "MessengTo.h"
#include "Enemy.h"

void EnemyFarAttack0Task::Run(Enemy* enemy)
{
}

uint32_t EnemyFarAttack0Task::JudgePriority(const int id)
{
	std::shared_ptr<CharacterAI> enemy = MESSENGER.CallEnemyInstance(id);
	uint32_t attackCount = static_cast<uint32_t>(enemy->GetJudgeElement().attackCount);
	if (attackCount <= m_attackRatio)
		return maxPriority;

	return minPriority;
}
