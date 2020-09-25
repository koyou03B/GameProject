#pragma once
#include "EnemyBehaviorTask.h"

class EnemyFarAttack1Task : public EnemyBehaviorTask
{
public:
	EnemyFarAttack1Task() = default;
	~EnemyFarAttack1Task() = default;
	void Run();
	uint32_t JudgePriority();
};