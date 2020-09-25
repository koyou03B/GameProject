#pragma once
#include "EnemyBehaviorTask.h"

class EnemyNearAttack2Task : public EnemyBehaviorTask
{
public:
	EnemyNearAttack2Task() = default;
	~EnemyNearAttack2Task() = default;
	void Run();
	uint32_t JudgePriority();
};