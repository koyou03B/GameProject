#include "EnemyFarSpecialAttack0Task.h"
#include "Enemy.h"

void EnemyFarSpecialAttack0Task::Run(Enemy* enemy)
{
}

uint32_t EnemyFarSpecialAttack0Task::JudgePriority(const int id)
{
	std::shared_ptr<CharacterAI> enemy = MESSENGER.CallEnemyInstance(id);
	bool isWrath = enemy->GetEmotion().wrathParm.isWrath;
	if(!isWrath)
		return minPriority;

	return m_priority;
}
