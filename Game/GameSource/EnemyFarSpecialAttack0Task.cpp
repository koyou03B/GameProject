#include "EnemyFarSpecialAttack0Task.h"
#include "Enemy.h"

void EnemyFarSpecialAttack0Task::Run(Enemy* enemy)
{
}

uint32_t EnemyFarSpecialAttack0Task::JudgePriority(const int id, const VECTOR3F playerPos) 
{
	EnemyType type = static_cast<EnemyType>(id);
	CharacterAI* enemy = MESSENGER.CallEnemyInstance(type);

	bool isWrath = enemy->GetEmotion().wrathParm.isWrath;
	if(!isWrath)
		return minPriority;

	return m_priority;
}
