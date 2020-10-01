#include "EnemyNearAttack1Task.h"
#include "MessengTo.h"
#include "Enemy.h"

void EnemyNearAttack1Task::Run(Enemy* enemy)
{

}

uint32_t EnemyNearAttack1Task::JudgePriority(const int id)
{
	auto player = MESSENGER.CallPlayersInstance();
	std::shared_ptr<CharacterAI> enemy = MESSENGER.CallEnemyInstance(id);
	int targetID = enemy->GetJudgeElement().targetID;

	VECTOR3F playerPosition = player.at(targetID)->GetWorldTransform().position;
	VECTOR3F enemyPosition = enemy->GetWorldTransform().position;

	float direction = ToDistVec3(playerPosition - enemyPosition);
	VECTOR3F normalizeDist = NormalizeVec3(playerPosition - enemyPosition);

	VECTOR3F angle = player.at(targetID)->GetWorldTransform().angle;
	VECTOR3F front = VECTOR3F(sinf(angle.y), 0.0f, cosf(angle.y));
	front = NormalizeVec3(front);

	float dot = DotVec3(front, normalizeDist);

	float cosTheta = acosf(dot);

	if (dot <= m_frontRatio)
	{
		uint32_t attackHitCount = static_cast<uint32_t>(enemy->GetJudgeElement().attackHitCount);
		if (attackHitCount > m_attackRatio)
			return maxPriority;
	}

	return minPriority;
}
