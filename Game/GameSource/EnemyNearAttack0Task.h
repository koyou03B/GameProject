#pragma once
#include "EnemyBehaviorTask.h"

class EnemyNearAttack0Task : public EnemyBehaviorTask
{
public:
	EnemyNearAttack0Task() = default;
	~EnemyNearAttack0Task() = default;
	void Run();
	uint32_t JudgePriority();

};