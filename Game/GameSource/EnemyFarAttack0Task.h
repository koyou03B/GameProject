#pragma once
#include "EnemyBehaviorTask.h"

class EnemyFarAttack0Task : public EnemyBehaviorTask
{
public:
	EnemyFarAttack0Task() = default;
	~EnemyFarAttack0Task() = default;
	void Run();
	uint32_t JudgePriority();
};