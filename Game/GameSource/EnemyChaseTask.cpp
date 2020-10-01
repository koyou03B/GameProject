#include "EnemyChaseTask.h"
#include "MessengTo.h"
#include "Enemy.h"

void EnemyChaseTask::Run(Enemy* enemy)
{
	auto players = MESSENGER.CallPlayersInstance();

	int targetID = enemy->GetJudgeElement().targetID;
	uint32_t targetAttackCount = players.at(targetID)->GetJudgeElement().attackCount;

	int selectPlayer = 0;
	int playerCount = static_cast<int>(players.size());
	for (int i = 0; i < playerCount; ++i)
	{
		if (i != targetID)
		{
			uint32_t attackCount = players.at(i)->GetJudgeElement().attackCount;
			if (targetAttackCount <= attackCount)
				selectPlayer = i;
		}
	}
	//Setter
	enemy->GetJudgeElement().targetID = selectPlayer;
}

uint32_t EnemyChaseTask::JudgePriority(const int id)
{
	return m_priority;
}
