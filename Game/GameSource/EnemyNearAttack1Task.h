#pragma once
#include "EnemyBehaviorTask.h"

class EnemyNearAttack1Task : public EnemyBehaviorTask
{
public:
	EnemyNearAttack1Task() = default;
	~EnemyNearAttack1Task() = default;
	void Run();
	uint32_t JudgePriority();
};